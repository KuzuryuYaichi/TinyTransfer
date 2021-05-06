#ifndef _STRUCT_NET_COMMON_H
#define _STRUCT_NET_COMMON_H

#include <QDateTime>

enum DATA_TYPE
{
	DT_WB_FFT = 0x10001260,
	DT_WB_DDC = 0x10001001,
	DT_NB_DDC = 0x10003001,
	DT_NB_DEMODULATE = 0x10003002,
	DT_WB_RESULT = 0x10005001,
	DT_WB_CX_RESULT = 0x10005002
};

#pragma pack(1)

struct PACK_HEAD
{
	qint32 iPackHead = 0x66666666;
	qint32 iPackType;//��������
	qint16 iPackSerial;//����� 0-65535ѭ��
	qint16 iPackTotal;//�ְ�����
	qint16 iPackSubNum;//�ְ����
	qint16 iDataLength;//���ݳ���
};

//a)SYSTEMTIME - C++ϵͳʱ��ṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//Int16	wYear	��	2
//Int16	wMonth	��	2
//Int16	wDayOfWeek	��	2
//Int16	wDay	��	2
//Int16	wHour	Сʱ	2
//Int16	wMinute	����	2
//Int16	wSecond	����	2
//Int16	wMilliseconds	������	2
//�ܼƣ�	16
struct SYSTEM_TIME
{
	qint16 wYear;
	qint16 wMonth;
	qint16 wDayOfWeek;
	qint16 wDay;
	qint16 wHour;
	qint16 wMinute;
	qint16 wSecond;
	qint16 wMillisecond;

	void calc()
	{
		QDateTime t = QDateTime::currentDateTime();
		auto date = t.date();
		wYear = date.year();
		wMonth = date.month();
		wDay = date.day();
		wDayOfWeek = date.dayOfWeek();
		auto time = t.time();
		wHour = time.hour();
		wMinute = time.minute();
		wSecond = time.second();
		wMillisecond = time.msec();
	}

	void calc(char* t)
	{
		static qint16 MONTH_DAYS[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		wYear = 2000 + ((t[0] >> 2) & 0x3F);
		if ((wYear % 4 == 0 && wYear % 100 != 0) || wYear % 400 == 0)
		{
			MONTH_DAYS[1] = 29;
		}
		qint16 wDay_offset = ((t[0] & 0x3) << 7) | ((t[1] >> 1) & 0x7F);
		for (wMonth = 0; wDay_offset > 0; ++wMonth)
		{
			if (wDay_offset - MONTH_DAYS[wMonth] > 0)
				wDay_offset -= MONTH_DAYS[wMonth];
			else
			{
				++wMonth;
				break;
			}
		}
		wDay = wDay_offset;
		wHour = ((t[1] & 0x1) << 4) | (t[2] >> 4) & 0xF;
		wMinute = ((t[2] & 0xF) << 2) | ((t[3] >> 6) & 0x3);
		wSecond = (t[3] & 0x3F);
	}
};

//��2 	ϵͳʱ��
//b)	SAMPLETIME - ΢��ʱ��ṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//SYSTEMTIME
//sysTime	ϵͳʱ��	16
//Int32	dwMicroSecond	΢����	4
//�ܼƣ�	20
struct SAMPLE_TIME
{
	SYSTEM_TIME sysTime;
	qint32 dwMicroSecond;
};

//��3 	΢��ʱ��
//c)	ACCURATETIME�C����ʱ��ṹ��
//���η���	�ֶ�����	��ʾ����	��׼�ֽ���
//SYSTEMTIME
//sysTime	ϵͳʱ��	16
//Int16	iMicrosecond	΢����	2
//Int16	iNanosecond	������	2
//�ܼƣ�	20
struct ACCURATE_TIME
{
	SYSTEM_TIME sysTime;
	qint16 iMicrosecond;
	qint16 iNanosecond;
};

#pragma pack()

#endif
