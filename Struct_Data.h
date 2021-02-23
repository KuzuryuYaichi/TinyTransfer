#ifndef _STRUCT_DATA_H_
#define _STRUCT_DATA_H_

#include <memory>

#pragma pack(1)

typedef short IQ_Data[2];

struct Struct_NB
{
	int OrderNum;
	char time1[4];
	int time2;
	char type;
	char gain;
	short GPS_BD1;
	int BD_GPS2;
	__int64 DataValid;
	IQ_Data Data[64];

	Struct_NB() {}

	Struct_NB(unsigned char* buffer)
	{
		memcpy(this, buffer, sizeof(Struct_NB));
	}
};

struct Struct_WB
{
	int OrderNum;
	int time1;
	int time2;
	char type;
	char gain;
	short GPS_BD1;
	int BD_GPS2;
	char DigitalGain[60];
	IQ_Data Data[64];

	Struct_WB(unsigned char* buffer)
	{
		OrderNum = *(int*)(buffer + 0);
		time1 = *(int*)(buffer + 4);
		time2 = *(int*)(buffer + 8);
		type = *(char*)(buffer + 12);
		gain = *(char*)(buffer + 13);
		GPS_BD1 = *(short*)(buffer + 14);
		BD_GPS2 = *(int*)(buffer + 16);
		unsigned char* buf = buffer + 20;
		for (int i = 0; i < 57; ++i)
		{
			DigitalGain[i] = buffer[i];
		}

		buf = buffer + 80;
		for (int i = 0; i < 57; ++i)
		{
			Data[i][0] = (short)((buf[i] << 8) | buf[i + 1]);
			Data[i][1] = (short)((buf[i + 2] << 8) | buf[i + 3]);
			buf += 4;
		}
	}
};

static const int FFT_NUM_MAP[] = { 400, 800, 1600, 3200, 200 };

struct Struct_FFT
{
	static const int PACK_LEN = (200 * 2 + 10 * 8) * 16;
	short Order;
	short identify;
	char time1[4];
	int time2;
	char type;
	char gain;
	short GPS_BD1;
	int BD_GPS2;
	char pointNum;
	char bandNum;
	char DigitalGain[57];
	char remark;
	short Data[PACK_LEN];

	Struct_FFT(unsigned char* buffer)
	{
		memcpy(this, buffer, 80 + 2 * FFT_NUM_MAP[buffer[20]]);
	}
};

#pragma pack()

#endif
