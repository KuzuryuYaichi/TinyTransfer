#ifndef _FFT_PACK_
#define _FFT_PACK_

#include <memory>
#include "Struct_Data.h"
#include "FFT_PACK_DATA.h"
#include <QMap>

class FFT_PACK
{
private:
    static const int DATA_LENGTH = 3200 * 60;
    static QMap<short, std::shared_ptr<FFT_PACK>> dict;
    static int lastPointNum;

	bool isFill[57];

public:
	qint32 pointNum;
    short data[DATA_LENGTH];
	struct FFT_PACK_DATA fftPackData;

    static std::shared_ptr<FFT_PACK> DataProcessFFT(struct Struct_FFT* ptr);
    FFT_PACK(struct Struct_FFT* ptr);
    bool push(struct Struct_FFT* ptr);
    int MakeProtocol(int pack_order);
};

#endif
