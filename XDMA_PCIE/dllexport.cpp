#include "pch.h"

#include "dllexport.h"
#include "stream_dma.hpp"
#include "streaming_dma.h"
#include <array>
#include <numeric>
#include <thread>
#include <vector>

xdma_device* pdev[3] = { nullptr };

PDATA_CALLBACK CallbackNB = nullptr, CallbackWB = nullptr, CallbackFFT = nullptr;

int OpenDevice()
{
    const auto device_paths = get_device_paths(GUID_DEVINTERFACE_XDMA);
    if (device_paths.empty()) {
		return -1;
    }

	SetIsRunning(true);
	int res = 0;
	for (int i = 0; i < device_paths.size(); i++)
	{
		pdev[i] = new xdma_device(device_paths[i]);
		ReadThread(*pdev[i], CallbackNB, CallbackWB, CallbackFFT);
	}
	return res;
}

int CloseDevice()
{
	SetIsRunning(false);
	for (int i =0; i < 2; i++)
	{
		if(pdev[i] != nullptr)
			delete pdev[i];
	}
    return 0;
}

int WriteData_AIS_Interrupt(unsigned char* data, int len)
{
	unsigned char* data2 = new unsigned char[20];
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			data2[i * 4 + j] = data[4 * i + 4 - 1 - j];
		}
	}
	unsigned int* Data = (unsigned int*)data2;
	for (int i = 0; i < 5; ++i)
	{
		WriteUserRegister(0x14, Data[i]);
	}
	delete[] data2;
    return 0;
}

int WriteData_ADS_Interrupt(unsigned char* data, int len)
{	
	unsigned char* data2 = new unsigned char[20];
	for (int i=0;i< 5;i++ )
	{
		for (int j = 0; j < 4; j++)
		{
			data2[i * 4 + j] = data[4 * i + 4 - 1 - j];
		}	
	}
	unsigned int* Data = (unsigned int*)data2;
	for (int i = 0; i < 5; i++)
	{
		WriteUserRegister(0x14, Data[i]);
	}
	delete[] data2;
    return 0;
}

void RegisterCallBackNB(PDATA_CALLBACK pfunc, unsigned char* pref)
{
    CallbackNB = pfunc;
}

void RegisterCallBackWB(PDATA_CALLBACK pfunc, unsigned char* pref)
{
	CallbackWB = pfunc;
}

void RegisterCallBackFFT(PDATA_CALLBACK pfunc, unsigned char* pref)
{
    CallbackFFT = pfunc;
}

int StopCallbackFunc()
{
    CallbackNB = nullptr;
	CallbackWB = nullptr;
    CallbackFFT = nullptr;
	return 0;
}

void WriteUserRegister(long addr, unsigned int value)
{
	pdev[0]->write_user_register(addr, value);
}

unsigned int ReadUserRegister(long addr)
{
	return pdev[0]->read_user_register(addr);
}

void SetDataLenFFT(int len)
{
	SetPackLenFFT(len);
}
