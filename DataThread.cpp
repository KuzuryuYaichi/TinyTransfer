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
#include "SerialPort/qextserialenumerator.h"

extern threadsafe_queue<std::shared_ptr<struct Struct_NB>> tsqueueNB;
extern threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_NB>>> tsqueueNBs;
extern threadsafe_queue<std::shared_ptr<struct Struct_WB>> tsqueueWB;
extern threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_WB>>> tsqueueWBs;
extern threadsafe_queue<std::shared_ptr<struct Struct_FFT>> tsqueueFFT;
extern threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_FFT>>> tsqueueFFTs;
extern threadsafe_queue<std::shared_ptr<struct Struct_Orders>> tsqueueSerialOrder;

struct NB_Params nb_params[64];

DataThread::DataThread(QObject* parent) : QThread(parent) {}

void DataThread::SetUdpSocket(M_UdpSocket* udpSocket)
{
	this->udpSocket = udpSocket;
}

DataThreadNB::DataThreadNB(QObject* parent): DataThread(parent)
{
	for (int i = 0; i < 64; ++i)
	{
		Data[i].iDDCChan = i;
	}
}

void DataThreadNB::run()
{
	while (true)
	{
		//auto ptr = tsqueueNB.wait_and_pop();
		//MakeProtocol(PROT_DDC, ptr);

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
		//auto ptr = tsqueueFFT.wait_and_pop();
		//MakeProtocol(PROT_FFT, ptr);

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
	winQextSerialPort = new Win_QextSerialPort(QString::fromLocal8Bit("COM3"));
	if (!winQextSerialPort->open(QIODevice::ReadWrite))
		qDebug() << "COM3 Open Failed!";
	winQextSerialPort->setBaudRate(BaudRateType::BAUD115200);
	winQextSerialPort->setDataBits(DataBitsType::DATA_8);
	winQextSerialPort->setFlowControl(FlowType::FLOW_OFF);
	winQextSerialPort->setParity(ParityType::PAR_NONE);
	winQextSerialPort->setStopBits(StopBitsType::STOP_1);

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
				order[1] = 5;
				QByteArray info = SerialWriteRead(order, 7);
				char* retData = info.data();
				*(short*)(ret.cReserved + 6) |= 1 << 1;
				break;
			}
			case RF_STATUS:
			{
				QByteArray info = SerialWriteRead(order, 7);
				char* retData = info.data();
				*(short*)(ret.cReserved + 6) |= ((retData[1] == 5) ? 1 : 0) << 2;
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
	//QTimer timer;
	//timer.start(1500);
	winQextSerialPort->write(data, len);
	winQextSerialPort->flush();
	QByteArray info;
	info.append(winQextSerialPort->readAll());
	//while (info.size() < 5)
	//	info.append(winQextSerialPort->read(5 - info.size()));
	//timer.stop();
	//qDebug() << "Elapsed Time: " << 1500 - timer.remainingTime() << "ms\r\n";
	return info;
}

void DataThreadWB::MakeProtocol(int type, struct Struct_WB* ptr)
{
	for (int i = 0; i < 10; ++i)
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

void DataThreadNB::MakeProtocol(int type, std::shared_ptr<struct Struct_NB>& ptr)
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

void DataThreadFFT::MakeProtocol(int type, std::shared_ptr<struct Struct_FFT>& ptr)
{
	int fft_point_num = FFT_NUM_MAP[ptr->pointNum];
	if (fft_point_num <= ptr->PACK_LEN)
	{
		FFT_DATA fft_data(ptr);
		int len = fft_data.MakeNetProtocol(ptr);
		if (udpSocket != nullptr)
			udpSocket->udpDataSend((char*)&fft_data, len);
	}
}
