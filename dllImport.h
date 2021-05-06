#ifndef _DLL_IMPORT_H
#define _DLL_IMPORT_H

typedef void(__stdcall* PDATA_CALLBACK)(void* pBuffer, int PACK_LEN, int PACK_NUM);

void __stdcall DataNB(void* buffer, int PACK_LEN, int PACK_NUM);

void __stdcall DataFFT(void* buffer, int PACK_LEN, int PACK_NUM);

void __stdcall DataWB(void* buffer, int PACK_LEN, int PACK_NUM);

#endif
