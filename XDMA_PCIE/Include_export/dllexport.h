#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#include <windows.h>

typedef void (WINAPI *PDATA_CALLBACK)(void* pBuffer, int PACK_LEN, int PACK_NUM);

#define EXPORT extern "C" __declspec(dllexport)

EXPORT int OpenDevice();

EXPORT int CloseDevice();

EXPORT void RegisterCallBackWBNB(PDATA_CALLBACK pfunc, unsigned char* pref);

EXPORT void RegisterCallBackFFT(PDATA_CALLBACK pfunc, unsigned char* pref);

EXPORT int StopCallbackFunc();

EXPORT void WriteUserRegister(long addr, unsigned int value);

EXPORT unsigned int ReadUserRegister(long addr);

EXPORT void SetDataLenFFT(int len);
