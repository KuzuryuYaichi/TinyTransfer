#include "DataThread.h"
#include "ThreadSafeQueue.h"
#include "Struct_Data.h"
#include "M_UdpSocket.h"
#include "Struct_Order.h"
#include "Struct_NetData.h"
#include "Struct_Datas.h"
#include "dllexport.h"
#include "FFT_PACK.h"
#include <math.h>

extern threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_NB>>> tsqueueNBs;
extern threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_WB>>> tsqueueWBs;
extern threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_FFT>>> tsqueueFFTs;
extern threadsafe_queue<std::shared_ptr<struct Struct_Orders>> tsqueueSerialOrder;

struct NB_Params nb_params[64];
struct NB_Params wb_params;
double mgcMulVal[61];
bool IQ_Pos[64] = { false };
int NB_AGC_Sum[64] = { 0 };
short agcCmp[] = { 0x54B0, 0x4B7B, 0x4345, 0x3BF5,
	0x356F, 0x2FA0, 0x2A72, 0x25D4, 0x21B7, 0x1E0D, 0x1AC8, 0x17DE, 0x1546,
	0x12F6, 0x10E6, 0x0F0F, 0x0D6C, 0x0BF6, 0x0AA9, 0x0981, 0x0878, 0x078C,
	0x068A, 0x05FF, 0x0558, 0x04C3, 0x043F, 0x03C8, 0x035F, 0x0301, 0x02AE,
	0x0263, 0x0221, 0x01E5, 0x01B1, 0x0182, 0x0158, 0x0132, 0x0111, 0x00F3,
	0x00D9, 0x00C1, 0x00AC, 0x0099, 0x0089, 0x007A, 0x006D, 0x0061, 0x0056,
	0x004D, 0x0045, 0x003D, 0x0036, 0x0031, 0x002B, 0x0027, 0x0022, 0x001F,
	0x001B, 0x0018, 0x0016, 0x0013, 0x0011, 0x000F };

DataThread::DataThread(QObject* parent) : QThread(parent) {}

void DataThread::SetUdpSocket(M_UdpSocket* udpSocket)
{
	this->udpSocket = udpSocket;
}

DataThreadNB::DataThreadNB(QObject* parent): DataThread(parent)
{
	for (int i = 0; i < 32; ++i)
	{
		auto temp = agcCmp[i];
		agcCmp[i] = agcCmp[63 - i];
		agcCmp[63 - i] = temp;
	}

	for (int i = 0; i < 64; ++i)
	{
		Data[i].iDDCChan = i;
		nb_params[i].mgcVal = 0; // 初始的增益值
	}
	
	for (int i = 0; i < 61; ++i) // 0-60的增益倍数
	{
		mgcMulVal[i] = pow(10.0, i / 20.0);
	}
}

void DataThreadNB::run()
{
	while (true)
	{
		auto ptr = tsqueueNBs.wait_and_pop();
		for (int i = 0; i < ptr->pos; ++i)
		{
			MakeProtocol(PROT_DDC, ptr->p[i]);
		}
	}
}

DataThreadWB::DataThreadWB(QObject* parent) : DataThread(parent)
{
	for (int i = 0; i < 60; ++i)
	{
		Data[i].iDDCChan = i;
		Record[i].iDDCChan = i;
	}
}

void DataThreadWB::run()
{
	while (true)
	{
		auto ptr = tsqueueWBs.wait_and_pop();
		for (int i = 0; i < ptr->pos; ++i)
		{
			MakeProtocol(PROT_DDC, ptr->p[i]);
		}
	}
}

DataThreadFFT::DataThreadFFT(QObject* parent) : DataThread(parent) {}

void DataThreadFFT::run()
{
	while (true)
	{
		auto ptr = tsqueueFFTs.wait_and_pop();
		for (int i = 0; i < ptr->pos; ++i)
		{
			MakeProtocol(PROT_FFT, ptr->p[i]);
		}
	}
}

OrderSerialThread::OrderSerialThread(QObject* parent) : DataThread(parent) {}

void OrderSerialThread::run()
{
	qSerialPort = new QSerialPort(QString::fromLocal8Bit("COM3"));
	if (!qSerialPort->open(QIODevice::ReadWrite))
		qDebug() << "COM3 Open Failed!";
	qSerialPort->setBaudRate(QSerialPort::Baud115200);
	qSerialPort->setDataBits(QSerialPort::Data8);
	qSerialPort->setFlowControl(QSerialPort::NoFlowControl);
	qSerialPort->setParity(QSerialPort::NoParity);
	qSerialPort->setStopBits(QSerialPort::OneStop);

	while (true)
	{
		auto ptr = tsqueueSerialOrder.wait_and_pop();
		SerialOrderDeal(ptr);
	}
}

bool isRecord = false;

void OrderSerialThread::SerialOrderDeal(std::shared_ptr<struct Struct_Orders> ptr)
{
	S_Control_Instruction ret(ptr->instruction_Net, ptr->channel);
	switch (ptr->instruction_Net)
	{
	case NET_SELF_CHECK:
	{
		static bool hasRfDesc = false, hasRfStatus = false;
		char orgDescVal;
		for (int i = 0; i < ptr->pos; ++i)
		{
			char* order = ptr->p[i]->order;
			switch (order[0])
			{
			case FPGA_TEMPRATURE:
			{
				QByteArray info = SerialWriteRead(order, 7);
				unsigned char* retData = (unsigned char*)info.data();
				*(unsigned short*)(ret.cReserved + 4) = (retData[1] << 8) | retData[2];
				break;
			}
			case RF_AGC_MGC_SWITCH:
			{
				order[1] = 0;
				QByteArray info = SerialWriteRead(order, 7);
				char* retData = info.data();
				*(short*)(ret.cReserved + 6) |= 1 << 0;
				break;
			}
			case RF_DESC_CONTROL:
			{
				if (!hasRfDesc)
				{
					order[1] = 5;
					QByteArray info = SerialWriteRead(order, 7);
					char* retData = info.data();
					*(short*)(ret.cReserved + 6) |= 1 << 1;
				}
				else
				{
					order[1] = orgDescVal;
					QByteArray info = SerialWriteRead(order, 7);
				}
				hasRfDesc = !hasRfDesc;
				break;
			}
			case RF_STATUS:
			{
				if (!hasRfStatus)
				{
					QByteArray info = SerialWriteRead(order, 7);
					char* retData = info.data();
					orgDescVal = retData[1];
				}
				else
				{
					QByteArray info = SerialWriteRead(order, 7);
					char* retData = info.data();
					*(short*)(ret.cReserved + 6) |= ((retData[1] == 5) ? 1 : 0) << 2;
				}
				hasRfStatus = !hasRfStatus;
				break;
			}
			case PCIE_STATUS:
			{
				QByteArray info = SerialWriteRead(order, 7);
				char* retData = info.data();
				*(short*)(ret.cReserved + 6) |= retData[1] << 3;
				break;
			}
			case CLOCK_STATUS:
			{
				QByteArray info = SerialWriteRead(order, 7);
				char* retData = info.data();
				*(short*)(ret.cReserved + 6) |= retData[1] << 4;
				break;
			}
			}
		}
		break;
	}
	case NET_RF_GAIN:
	{
		for (int i = 0; i < ptr->pos; ++i)
		{
			char* order = ptr->p[i]->order;
			switch (order[0])
			{
			case RF_AGC_MGC_SWITCH:
			{
				QByteArray info = SerialWriteRead(order, 7);
				break;
			}
			case RF_DESC_CONTROL:
			{
				QByteArray info = SerialWriteRead(order, 7);
				break;
			}
			case DIGITAL_MGC_PARAMS:
			{
				QByteArray info = SerialWriteRead(order, 7);
				ret.CopyRet(info.data());
				break;
			}
			case DIGITAL_NB_MGC_GAIN:
			{
				nb_params[order[1]].mgcVal = order[2];
				break;
			}
			}
		}
		break;
	}
	case NET_RBW:
	{
		char* order = ptr->p[0]->order;
		QByteArray info = SerialWriteRead(order, 7);
		SetDataLenFFT(order[1]);
		break;
	}
	case NET_ISRECORD:
	{
		isRecord = ptr->p[0]->order[1];
		break;
	}
	default:
	{
		if (ptr->pos > 0 && ptr->pos < 2)
		{
			QByteArray info = SerialWriteRead(ptr->p[0]->order, 7);
			ret.CopyRet(info.data());
		}
		break;
	}
	}
	if (udpSocket != nullptr)
	{
		udpSocket->udpDataSend((const char*)&ret, sizeof(ret), ptr->remoteAddr, ptr->remotePort);
	}
}

QByteArray OrderSerialThread::SerialWriteRead(const char* data, qint64 len)
{
	int wrLen = qSerialPort->write(data, len);
	QByteArray info;
	if (qSerialPort->waitForBytesWritten(100)) {
		if (qSerialPort->waitForReadyRead(100)) {
			info = qSerialPort->readAll();
			while (qSerialPort->waitForReadyRead(10))
				info += qSerialPort->readAll();
		}
		else {
			qDebug() << tr("Wait read response timeout %1").arg(QTime::currentTime().toString());
		}
	}
	else {
		qDebug() << tr("Wait write request timeout %1").arg(QTime::currentTime().toString());
	}
	msleep(10);
	return info;
}

void DataThreadWB::MakeProtocol(int type, struct Struct_WB* ptr)
{
	for (int i = 0; i < 58; ++i)
	{
		if (Data[i].add(ptr, WB_Pos))
		{
			Record[i].WriteFile((const char*)Data[i].Data);
			//int len = Data[i].MakeNetProtocol(ptr);
			//if (udpSocket != nullptr)
			//	udpSocket->udpDataSend((char*)(Data + i), len);
		}
	}
}

void DataThreadNB::MakeProtocol(int type, struct Struct_NB* ptr)
{
	for (int i = 0; i < 64; ++i)
	{
		if (Data[i].add(ptr, NB_Pos))
		{
			int len = Data[i].MakeNetProtocol(ptr);
			if (udpSocket != nullptr)
				udpSocket->udpDataSend((char*)(Data + i), len);
		}
	}
}

void DataThreadFFT::MakeProtocol(int type, struct Struct_FFT* ptr)
{
	int fft_point_num = FFT_NUM_MAP[ptr->pointNum];
	if (fft_point_num <= ptr->PACK_LEN)
	{
		auto fft_pack = FFT_PACK::DataProcessFFT(ptr);
		if (fft_pack != nullptr)
		{
			for (int i = 0; i < 6; ++i)
			{
				int len = fft_pack->MakeProtocol(i);
				if (udpSocket != nullptr)
					udpSocket->udpDataSend((char*)&fft_pack->fftPackData, len);
			}
		}
	}
}
