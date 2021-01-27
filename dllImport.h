#pragma once

typedef void(__stdcall* PDATA_CALLBACK)(void* pBuffer, int PACK_LEN, int PACK_NUM);

void __stdcall DataWBNB(void* buffer, int PACK_LEN, int PACK_NUM);

void __stdcall DataFFT(void* buffer, int PACK_LEN, int PACK_NUM);
