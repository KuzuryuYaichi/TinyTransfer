#ifndef _FFT_PACK_DATA_H
#define _FFT_PACK_DATA_H

#include <memory>
#include "Struct_NetData.h"

#pragma pack(1)

struct FFT_PACK_DATA
{
	static const int PACK_LEN = 3200 * 2 * 10;
	PACK_HEAD packHead;
	qint32 iDDCChan;
	SAMPLE_TIME iSampleTime;
	FFT_PARAM struPara;
	qint32 iDDCDataLen;
	qint16 ifData[PACK_LEN];
	qint32 packTail;

	FFT_PACK_DATA(Struct_FFT* ptr)
	{
		iDDCDataLen = FFT_NUM_MAP[ptr->pointNum];
		int pack_len = sizeof(int) + sizeof(SAMPLE_TIME) + sizeof(FFT_PARAM) + sizeof(int) + FFT_NUM_MAP[ptr->pointNum] * 2 * 10;
		packHead.iPackType = DT_WB_FFT;
		packHead.iPackSerial = ptr->identify;
		packHead.iPackTotal = 6;
		packHead.iPackSubNum = ptr->bandNum;
		packHead.iDataLength = pack_len;
		struPara.make(ptr);
		iDDCChan = ptr->bandNum;
		iSampleTime.sysTime.calc(ptr->time1);
		iSampleTime.dwMicroSecond = ptr->time2;
	}

	int MakeNetProtocol(short* iData, int pointNum, int packOrder)
	{
		char* buf = (char*)this;
		int pack_len = sizeof(int) + sizeof(SAMPLE_TIME) + sizeof(FFT_PARAM) + sizeof(int) + FFT_NUM_MAP[pointNum] * 2 * 10;
		packHead.iPackSubNum = packOrder;
		memcpy(ifData, iData + packOrder * FFT_NUM_MAP[pointNum] * 10, FFT_NUM_MAP[pointNum] * 2 * 10);
		*(int*)(buf + 16 + pack_len) = 0x77777777;
		return 20 + pack_len;
	}
};

#pragma pack()

#endif
