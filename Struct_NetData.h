#ifndef _STRUCT_NET_DATA_H_
#define _STRUCT_NET_DATA_H_

#include "Struct_Data.h"
#include "Struct_Order.h"
#include "Struct_NetCommon.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>

extern struct NB_Params nb_params[64];

#pragma pack(1)

//h)	NB_DDC_PARAM – NB 参数数据结构体
//修饰符号	字段名称	表示内容	标准字节数
//int	iFreq	频率	4
//int	iBandwidth	带宽	4 单位HZ 具体数值
//int	iGainMode	增益模式 : 0手动MGC，大于0（1, 2, 3, 4……）为AGC自动。如果有MGC和AGC混合模式，将在指令中体现，这里不做具体约定。	4
//int	iMgc	手动增益值	4
//int	iAgc_Val	自动增益值	4
//int	iBfo	拍频	4
//int	iSampling	采样率	4
//char[]	iWorkMode[20]	工作方式	20
//int	iBiasToCenFreq	与中心频率的偏差值	4
//Int32	iDataMode	0:IQ 1 : AM 2 : ISB 3 : CW  4 : USB，5 : LSB	4
//总计：	56 Byte
struct NB_DDC_PARAM
{
	qint32 iFreq;
	qint32 iBandwidth;
	qint32 iGainMode;
	qint32 iMgc;
	qint32 iAgc_Val;
	qint32 iBfo;
	qint32 iSampling;
	qint8 iWorkMode[20];
	qint32 iBiasToCenFreq;
	qint32 iDataMode;//默认CW

	void make(struct Struct_NB* ptr, int i)
	{
		iFreq = nb_params[i].freq;
		iBandwidth = nb_params[i].band;
		iGainMode = 0;
		iMgc = ptr->gain;
		iAgc_Val = ptr->gain;
		iBfo = 2000;
		iSampling = nb_params[i].CaclSample();
		iBiasToCenFreq = 0;
		iDataMode = nb_params[i].mode;
	}

	void make(std::shared_ptr<struct Struct_NB>& ptr, int i)
	{
		make(&(*ptr), i);
	}
};

//i)	NB_DDC_DATA – NB 数据结构体
//修饰符号	字段名称	表示内容	标准字节数
//Int32	iDDCChan	通道号	4
//ACCURATETIME	iSampleTime	样点时刻(纳秒)	20
//NB_DDC_PARAM
//struPara	窄带参数值	56
//Int32	iSignalLevel	电平值，没有就填0.	4
//Int32	DataLen	DDC长度	4
//short[]	Data[512]	DDC实数数据	1024
//总计：	Byte
struct NB_DDC_DATA
{
	static const int PACK_LEN = 512;
	PACK_HEAD packHead;
	qint32 iDDCChan = 0;
	SAMPLE_TIME iSampleTime;
	NB_DDC_PARAM struPara;
	qint32	iSignalLevel = 0;
	qint32	DataLen = 0;
	IQ_Data Data[PACK_LEN];
	qint32 packTail;

	NB_DDC_DATA() {}

	bool add(struct Struct_NB* ptr, int pos[])//返回是否攒够了足够长度的发送数据
	{
		__int64 DataValid =
			((ptr->DataValid & 0xFF00000000000000) >> 56) |
			((ptr->DataValid & 0x00FF000000000000) >> 40) |
			((ptr->DataValid & 0x0000FF0000000000) >> 24) |
			((ptr->DataValid & 0x000000FF00000000) >> 8) |
			((ptr->DataValid & 0x00000000FF000000) << 8) |
			((ptr->DataValid & 0x0000000000FF0000) << 24) |
			((ptr->DataValid & 0x000000000000FF00) << 40) |
			((ptr->DataValid & 0x00000000000000FF) << 56);

		if (iDDCChan < 0 || iDDCChan > 63 || !(DataValid & (((__int64)1) << iDDCChan)))
			return false;
		memcpy(Data[pos[iDDCChan]], ptr->Data[iDDCChan], sizeof(IQ_Data));
		if (++pos[iDDCChan] == PACK_LEN)
		{
			pos[iDDCChan] = 0;
			iSampleTime.sysTime.calc();
			iSampleTime.dwMicroSecond = ptr->time2;
			struPara.make(ptr, iDDCChan);
			return true;
		}
		return false;
	}

	int MakeNetProtocol(struct Struct_NB* ptr)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(*this) - sizeof(int) - sizeof(PACK_HEAD);
		packHead.iPackType = DT_NB_DDC;
		packHead.iPackSerial = 0;
		packHead.iPackTotal = 0;
		packHead.iPackSubNum = 0;
		packHead.iDataLength = pack_len;
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}
	
	bool add(std::shared_ptr<struct Struct_NB>& ptr, int pos[])//返回是否攒够了足够长度的发送数据
	{
		__int64 DataValid =
			((ptr->DataValid & 0xFF00000000000000) >> 56) |
			((ptr->DataValid & 0x00FF000000000000) >> 40) |
			((ptr->DataValid & 0x0000FF0000000000) >> 24) |
			((ptr->DataValid & 0x000000FF00000000) >> 8) |
			((ptr->DataValid & 0x00000000FF000000) << 8) |
			((ptr->DataValid & 0x0000000000FF0000) << 24) |
			((ptr->DataValid & 0x000000000000FF00) << 40) |
			((ptr->DataValid & 0x00000000000000FF) << 56);

		if (iDDCChan < 0 || iDDCChan > 63 || !(DataValid & (((__int64)1) << iDDCChan)))
			return false;
		memcpy(Data[pos[iDDCChan]], ptr->Data[iDDCChan], sizeof(IQ_Data));
		if (++pos[iDDCChan] == PACK_LEN)
		{
			pos[iDDCChan] = 0;
			iSampleTime.sysTime.calc();
			iSampleTime.dwMicroSecond = ptr->time2;
			struPara.make(ptr, iDDCChan);
			return true;
		}
		return false;
	}

	int MakeNetProtocol(std::shared_ptr<Struct_NB>& ptr)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(*this);
		packHead.iPackType = DT_NB_DDC;
		packHead.iPackSerial = 0;
		packHead.iPackTotal = 0;
		packHead.iPackSubNum = 0;
		packHead.iDataLength = pack_len;
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}
};

//g)	WBZC_DDC_DATA - 宽带ZC数据结构体
//修饰符号	字段名称	表示内容	标准字节数
//Int32	iDDCChan	通道号	4
//Int32	iFs	采样速率	4
//Int32	iResolution	频率分辨率	4
//Int32	iScanFreqNum	接收机扫描频率数，
//1 - 定频； > 1扫频	4
//Int32	iScanningFlag	状态标记0未扫描1扫描	4
//Int32	iCenterFreq	宽带中心频率	4
//Int32	iBandWidth	数据带宽	4
//Int32	iGain	增益值，这里填写实际增益值。增益模式将在指令中体现。	4
//SAMPLETIME
//iSampleTime	采样时间(微妙)	20
//Int32	iSampleDataLen	IQ数据长度，并非字节数	4
//Int32[]	iIQData[8192]	IQ数据，高16位为Q，低16位为I，最大8K	32768

struct WB_DDC_DATA
{
	static const int PACK_LEN = 512;
	PACK_HEAD packHead;
	qint32 iDDCChan = 0;
	SAMPLE_TIME iSampleTime;
	//WB_DDC_PARAM struPara;
	qint32 iSignalLevel = 0;
	qint32 DataLen = 0;
	IQ_Data Data[PACK_LEN];
	qint32 packTail;

	WB_DDC_DATA() {}

	bool add(struct Struct_WB* ptr, int pos[])//返回是否攒够了足够长度的发送数据
	{
		if (iDDCChan < 0 || iDDCChan > 59)
			return false;
		memcpy(Data[pos[iDDCChan]], ptr->Data[iDDCChan], sizeof(IQ_Data));
		if (++pos[iDDCChan] == PACK_LEN)
		{
			pos[iDDCChan] = 0;
			iSampleTime.sysTime.calc();
			iSampleTime.dwMicroSecond = ptr->time2;
			//struPara.make(ptr, iDDCChan);
			return true;
		}
		return false;
	}

	int MakeNetProtocol(struct Struct_WB* ptr)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(*this) - sizeof(int) - sizeof(PACK_HEAD);
		packHead.iPackType = DT_WB_DDC;
		packHead.iPackSerial = 0;
		packHead.iPackTotal = 0;
		packHead.iPackSubNum = 0;
		packHead.iDataLength = pack_len;
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}

	bool add(std::shared_ptr<struct Struct_WB>& ptr, int pos[])//返回是否攒够了足够长度的发送数据
	{
		if (iDDCChan < 0 || iDDCChan > 59)
			return false;
		memcpy(Data[pos[iDDCChan]], ptr->Data[iDDCChan], sizeof(IQ_Data));
		if (++pos[iDDCChan] == PACK_LEN)
		{
			pos[iDDCChan] = 0;
			iSampleTime.sysTime.calc();
			iSampleTime.dwMicroSecond = ptr->time2;
			//struPara.make(ptr, iDDCChan);
			return true;
		}
		return false;
	}

	int MakeNetProtocol(std::shared_ptr<Struct_WB>& ptr)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(*this);
		packHead.iPackType = DT_NB_DDC;
		packHead.iPackSerial = 0;
		packHead.iPackTotal = 0;
		packHead.iPackSubNum = 0;
		packHead.iDataLength = pack_len;
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}
};

extern bool isRecord;

struct WB_DDC_Record
{
	qint32 iDDCChan;
	QFile file;
	QString strDir;
	bool isRecord_dly = false;
	void WriteFile(const char* Data)
	{
		if (isRecord_dly != isRecord)
		{
			isRecord_dly = isRecord;
			if (isRecord_dly)
			{
				strDir = QCoreApplication::applicationDirPath() + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd");
				QDir dir;
				if (!dir.exists(strDir))
				{
					dir.mkdir(strDir);
				}
				file.setFileName(strDir + "/Channel_" + QString::number(iDDCChan + 1) + QDateTime::currentDateTime().toString("_hh_mm_ss") + ".dat");
				file.open(QIODevice::WriteOnly | QIODevice::Append);
			}
			else
			{
				file.close();
			}
		}
		if (isRecord)
		{
			file.write(Data, WB_DDC_DATA::PACK_LEN * sizeof(IQ_Data));
		}
	}
};

//e)	FFT_PARM –FFT参数结构体2
//修饰符号	字段名称	表示内容	标准字节数
//Int32	iFreq	中心频率	4
//Int32	iBandwidth	带宽	4
//Int32	iSampleRate	采样率（Hz）	4
//Int16	iGainMode	增益模式 : 0手动，大于0为自动（1, 2, 3, 4……）	1
//Int16	iMgcValue	手动增益值	2
//Int16	iAgcValue	自动增益值 2
//Int16	iWorkMode	射频工作方式	2
//Int16	iWindShape	窗函数（默认Blcakmann）
//0:Rect(矩形窗)
//1 : Hanning(汉宁窗)
//2 : Hamming（海明窗）
//3 : Blackmann(布莱克曼)
//4 : Gaussion(高斯窗)	2
//Int16	iEvenNum	平滑次数	2
//Int16	iOverlapDot	重叠点数	2
//总计：	24
struct FFT_PARAM
{
	qint32 iFreq;
	qint32 iBandwidth;
	qint32 iSampleRate;
	qint16 iGainMode;
	qint16 iMgcValue;
	qint16 iAgcValue;
	qint16 iWorkMode;
	qint16 iWindShape;
	qint16 iEvenNum;
	qint16 iOverlapDot;

	void make(struct Struct_FFT* ptr)
	{
		iFreq = 0;
		iBandwidth = 0;
		iSampleRate = 0;
		iGainMode = 0;
		iMgcValue = ptr->gain;
		iAgcValue = ptr->gain;
		iWorkMode = 1;
		iWindShape = 3;
		iEvenNum = 4;
		iOverlapDot = 0;
	}

	void make(std::shared_ptr<struct Struct_FFT>& ptr)
	{
		make(&(*ptr));
	}
};

//f)	FFT_DATA—FFT数据结构体
//修饰符号	字段名称	表示内容	标准字节数
//Int32	iDDCChan	通道号	4
//SAMPLETIME
//iSampleTime	样点时刻(微妙)	20
//FFT_PARAM
//struPara	FFT参数值	24
//Int32	iDDCDataLen	FFT数据长度	4
//short[]	ifData[]	FFT数据（频谱数据）	16384 变长
//总计：	16436变长

struct FFT_DATA
{
	static const int PACK_LEN = (200 * 2 + 10 * 8) * 16;
	PACK_HEAD packHead;
	qint32 iDDCChan;
	SAMPLE_TIME iSampleTime;
	FFT_PARAM struPara;
	qint32 iDDCDataLen;
	qint16 ifData[PACK_LEN];
	qint32 packTail;

	FFT_DATA(Struct_FFT* ptr)
	{
		iDDCDataLen = FFT_NUM_MAP[ptr->pointNum];
		for (int i = 0; i < iDDCDataLen; ++i)
		{
			ifData[i] = ptr->Data[i] - 10 * (19 - ptr->gain + 118.5);//将偏移量扩大10倍防止转换为整形的过程中丧失精度 由上位机最终还原
		}
	}

	FFT_DATA(std::shared_ptr<Struct_FFT>& ptr): FFT_DATA(&(*ptr))
	{
	}

	int MakeNetProtocol(std::shared_ptr<Struct_FFT>& ptr)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(int) + sizeof(SAMPLE_TIME) + sizeof(FFT_PARAM) + sizeof(int) + FFT_NUM_MAP[ptr->pointNum] * 2;
		packHead.iPackType = DT_WB_FFT;
		packHead.iPackSerial = ptr->identify;
		packHead.iPackTotal = 6;
		packHead.iPackSubNum = ptr->bandNum;
		packHead.iDataLength = pack_len;
		struPara.make(ptr);
		iDDCChan = ptr->bandNum;
		iSampleTime.sysTime.calc();
		iSampleTime.dwMicroSecond = ptr->time2;
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}

	int MakeNetProtocol(Struct_FFT* ptr)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(int) + sizeof(SAMPLE_TIME) + sizeof(FFT_PARAM) + sizeof(int) + FFT_NUM_MAP[ptr->pointNum] * 2;
		packHead.iPackType = DT_WB_FFT;
		packHead.iPackSerial = ptr->identify;
		packHead.iPackTotal = 6;
		packHead.iPackSubNum = ptr->bandNum;
		packHead.iDataLength = pack_len;
		struPara.make(ptr);
		iDDCChan = ptr->bandNum;
		iSampleTime.sysTime.calc();
		iSampleTime.dwMicroSecond = ptr->time2;
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}
};

#pragma pack()

#endif
