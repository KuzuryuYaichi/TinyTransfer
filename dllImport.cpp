#include "dllImport.h"
#include "ThreadSafeQueue.h"
#include "Struct_Data.h"
#include "Struct_Datas.h"

threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_NB>>> tsqueueNBs;
threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_WB>>> tsqueueWBs;
threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_FFT>>> tsqueueFFTs;

void DataNB(void* pBuffer, int PACK_LEN, int PACK_NUM)
{
	unsigned char* ptr = (unsigned char*)pBuffer;

	int pack_len = PACK_LEN / 32;
	auto pBuf_NB = std::make_shared<struct Struct_Datas<struct Struct_NB>>(PACK_NUM * 32);
	for (int i = 0; i < PACK_NUM; ++i)
	{
		//static unsigned int now = 0, last = 0;
		//now = *(unsigned int*)ptr;
		//if (now != last + 32)
		//{
		//	static int cc = 0;
		//	++cc;
		//}
		//else
		//{
		//	static int dd = 0;
		//	++dd;
		//}
		//last = now;

		unsigned char* pp = ptr;
		for (int i = 0; i < 32; ++i)
		{
			pBuf_NB->push(pp);
			pp += pack_len;
		}
		ptr += PACK_LEN;
	}
	tsqueueNBs.push(pBuf_NB);
}

const unsigned int PACK_LEN_PER_PACK[] = { 400 * 2 + 10 * 8, 800 * 2 + 10 * 8, 1600 * 2 + 10 * 8, 3200 * 2 + 10 * 8, 200 * 2 + 10 * 8 };

void DataFFT(void* pBuffer, int PACK_LEN, int PACK_NUM)
{
	unsigned char* ptr = (unsigned char*)pBuffer;

	auto pBuf_FFT = std::make_shared<struct Struct_Datas<struct Struct_FFT>>(PACK_NUM);
	for (int i = 0; i < PACK_NUM; ++i)
	{
		int fft_num = ptr[20];
		if (fft_num >= 0 && fft_num < 5)
		{
			if (PACK_LEN_PER_PACK[fft_num] == PACK_LEN)
			{
				pBuf_FFT->push(ptr);
				ptr += PACK_LEN;
			}
			else
				break;
		}
		else
		{
			++i;
			continue;
		}
	}
	tsqueueFFTs.push(pBuf_FFT);
}

void DataWB(void* pBuffer, int PACK_LEN, int PACK_NUM)
{
	unsigned char* ptr = (unsigned char*)pBuffer;
	int pack_len = PACK_LEN / 64;
	auto pBuf_WB = std::make_shared<struct Struct_Datas<struct Struct_WB>>(PACK_NUM * 64);
	for (int i = 0; i < PACK_NUM; ++i)
	{
		unsigned char* pp = ptr + 10 * 8;
		for (int i = 0; i < 64; ++i)
		{
			pBuf_WB->push(pp);
			pp += pack_len;
		}
		ptr += PACK_LEN + 10 * 8;
	}
	tsqueueWBs.push(pBuf_WB);
}
