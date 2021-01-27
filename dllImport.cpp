#include "dllImport.h"
#include "ThreadSafeQueue.h"
#include "Struct_Data.h"
#include "Struct_Datas.h"

threadsafe_queue<std::shared_ptr<struct Struct_NB>> tsqueueNB;
threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_NB>>> tsqueueNBs;
threadsafe_queue<std::shared_ptr<struct Struct_WB>> tsqueueWB;
threadsafe_queue<std::shared_ptr<struct Struct_FFT>> tsqueueFFT;
threadsafe_queue<std::shared_ptr<struct Struct_Datas<struct Struct_FFT>>> tsqueueFFTs;

void DataWBNB(void* pBuffer, int PACK_LEN, int PACK_NUM)
{
	unsigned char* ptr = (unsigned char*)pBuffer;

	//for (int i = 0; i < PACK_NUM; ++i)
	//{
	//	if (ptr[12] == 1)
	//	{
	//		auto pBuf = std::make_shared<struct Struct_NB>(*(struct Struct_NB*)ptr);
	//		tsqueueNB.push(pBuf);
	//	}
	//	else if (ptr[12] == 2)
	//	{
	//		std::shared_ptr<struct Struct_WB> pBuf(new struct Struct_WB(ptr));
	//		tsqueueWB.push(pBuf);
	//	}
	//	ptr += PACK_LEN;
	//}

	std::shared_ptr<struct Struct_Datas<struct Struct_NB>> pBuf_NB(new struct Struct_Datas<struct Struct_NB>(PACK_NUM * 32));
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

		if (ptr[12] == 1)
		{
			unsigned char* pp = ptr;
			for (int i = 0; i < 32; ++i)
			{
				pBuf_NB->push(pp);
				pp += 39 * 8;
			}	
		}
		else if (ptr[12] == 2)
		{

		}
		ptr += PACK_LEN;
	}
	tsqueueNBs.push(pBuf_NB);
}

const unsigned int PACK_LEN_PER_PACK[] = { 400 * 4 + 10 * 8, 800 * 4 + 10 * 8, 1600 * 4 + 10 * 8, 3200 * 4 + 10 * 8 };

void DataFFT(void* pBuffer, int PACK_LEN, int PACK_NUM)
{
	unsigned char* ptr = (unsigned char*)pBuffer;

	//for (int i = 0; i < PACK_NUM; ++i)
	//{
	//	std::shared_ptr<struct Struct_FFT> pBuf(new struct Struct_FFT(ptr));
	//	tsqueueFFT.push(pBuf);
	//	ptr += PACK_LEN;
	//}

	std::shared_ptr<struct Struct_Datas<struct Struct_FFT>> pBuf_FFT(new struct Struct_Datas<struct Struct_FFT>(PACK_NUM));
	for (int i = 0; i < PACK_NUM; ++i)
	{
		int fft_num = ptr[77];
		if (fft_num >= 0 && fft_num < 4)
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
