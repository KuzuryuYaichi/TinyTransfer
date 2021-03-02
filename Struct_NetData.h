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

//h)	NB_DDC_PARAM �C NB �������ݽṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//int	iFreq	Ƶ��	4
//int	iBandwidth	����	4 ��λHZ ������ֵ
//int	iGainMode	����ģʽ : 0�ֶ�MGC������0��1, 2, 3, 4������ΪAGC�Զ��������MGC��AGC���ģʽ������ָ�������֣����ﲻ������Լ����	4
//int	iMgc	�ֶ�����ֵ	4
//int	iAgc_Val	�Զ�����ֵ	4
//int	iBfo	��Ƶ	4
//int	iSampling	������	4
//char[]	iWorkMode[20]	������ʽ	20
//int	iBiasToCenFreq	������Ƶ�ʵ�ƫ��ֵ	4
//Int32	iDataMode	0:IQ 1 : AM 2 : ISB 3 : CW  4 : USB��5 : LSB	4
//�ܼƣ�	56 Byte
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
	qint32 iDataMode;//Ĭ��CW

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

//i)	NB_DDC_DATA �C NB ���ݽṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//Int32	iDDCChan	ͨ����	4
//ACCURATETIME	iSampleTime	����ʱ��(����)	20
//NB_DDC_PARAM
//struPara	խ������ֵ	56
//Int32	iSignalLevel	��ƽֵ��û�о���0.	4
//Int32	DataLen	DDC����	4
//short[]	Data[512]	DDCʵ������	1024
//�ܼƣ�	Byte
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

	bool add(struct Struct_NB* ptr, int pos[])//�����Ƿ��ܹ����㹻���ȵķ�������
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
	
	bool add(std::shared_ptr<struct Struct_NB>& ptr, int pos[])//�����Ƿ��ܹ����㹻���ȵķ�������
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

//g)	WBZC_DDC_DATA - ���ZC���ݽṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//Int32	iDDCChan	ͨ����	4
//Int32	iFs	��������	4
//Int32	iResolution	Ƶ�ʷֱ���	4
//Int32	iScanFreqNum	���ջ�ɨ��Ƶ������
//1 - ��Ƶ�� > 1ɨƵ	4
//Int32	iScanningFlag	״̬���0δɨ��1ɨ��	4
//Int32	iCenterFreq	�������Ƶ��	4
//Int32	iBandWidth	���ݴ���	4
//Int32	iGain	����ֵ��������дʵ������ֵ������ģʽ����ָ�������֡�	4
//SAMPLETIME
//iSampleTime	����ʱ��(΢��)	20
//Int32	iSampleDataLen	IQ���ݳ��ȣ������ֽ���	4
//Int32[]	iIQData[8192]	IQ���ݣ���16λΪQ����16λΪI�����8K	32768

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

	bool add(struct Struct_WB* ptr, int pos[])//�����Ƿ��ܹ����㹻���ȵķ�������
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

	bool add(std::shared_ptr<struct Struct_WB>& ptr, int pos[])//�����Ƿ��ܹ����㹻���ȵķ�������
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

//e)	FFT_PARM �CFFT�����ṹ��2
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//Int32	iFreq	����Ƶ��	4
//Int32	iBandwidth	����	4
//Int32	iSampleRate	�����ʣ�Hz��	4
//Int16	iGainMode	����ģʽ : 0�ֶ�������0Ϊ�Զ���1, 2, 3, 4������	1
//Int16	iMgcValue	�ֶ�����ֵ	2
//Int16	iAgcValue	�Զ�����ֵ 2
//Int16	iWorkMode	��Ƶ������ʽ	2
//Int16	iWindShape	��������Ĭ��Blcakmann��
//0:Rect(���δ�)
//1 : Hanning(������)
//2 : Hamming����������
//3 : Blackmann(��������)
//4 : Gaussion(��˹��)	2
//Int16	iEvenNum	ƽ������	2
//Int16	iOverlapDot	�ص�����	2
//�ܼƣ�	24
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

//f)	FFT_DATA��FFT���ݽṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//Int32	iDDCChan	ͨ����	4
//SAMPLETIME
//iSampleTime	����ʱ��(΢��)	20
//FFT_PARAM
//struPara	FFT����ֵ	24
//Int32	iDDCDataLen	FFT���ݳ���	4
//short[]	ifData[]	FFT���ݣ�Ƶ�����ݣ�	16384 �䳤
//�ܼƣ�	16436�䳤

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
			ifData[i] = ptr->Data[i] - 10 * (19 - ptr->gain + 118.5);//��ƫ��������10����ֹת��Ϊ���εĹ�����ɥʧ���� ����λ�����ջ�ԭ
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
