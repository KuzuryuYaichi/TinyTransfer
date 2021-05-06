#ifndef _DLL_EXPORT_H
#define _DLL_EXPORT_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef void (WINAPI *PDATA_CALLBACK)(void* pBuffer, int PACK_LEN, int PACK_NUM);

#define EXPORT extern "C" __declspec(dllexport)

EXPORT int OpenDevice();

EXPORT int CloseDevice();

EXPORT void RegisterCallBackNB(PDATA_CALLBACK pfunc, unsigned char* pref);

EXPORT void RegisterCallBackWB(PDATA_CALLBACK pfunc, unsigned char* pref);

EXPORT void RegisterCallBackFFT(PDATA_CALLBACK pfunc, unsigned char* pref);

EXPORT int StopCallbackFunc();

EXPORT void WriteUserRegister(long addr, unsigned int value);

EXPORT unsigned int ReadUserRegister(long addr);

EXPORT void SetDataLenFFT(int len);

#endif
