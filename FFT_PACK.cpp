#include "FFT_PACK.h"
#include "Struct_Data.h"

QMap<short, std::shared_ptr<FFT_PACK>> FFT_PACK::dict;

int FFT_PACK::lastPointNum = 0;

FFT_PACK::FFT_PACK(struct Struct_FFT* ptr): fftPackData(ptr)
{
    pointNum = ptr->pointNum;
    for (auto& f : isFill)
        f = false;
    push(ptr);
}

bool FFT_PACK::push(struct Struct_FFT* ptr)
{
    int pointNum = FFT_NUM_MAP[ptr->pointNum];
    if (lastPointNum != pointNum) //(重要)记录下上一次的PACK_LEN 以免变更分辨率后遇到和之前分辨率不一样而包序号一致的数据时导致的拼包显示错误
    {
        lastPointNum = pointNum;
        dict.clear();
        return false;
    }
    int channel = ptr->bandNum;
    if (channel >= 0 && channel < 57)
    {
        int baseOffset = channel * pointNum;
        short key = ptr->identify;
        if (dict.contains(key) && baseOffset + 2 * pointNum > DATA_LENGTH - 1)
        {
            dict.remove(key);
        }
        else
        {
            for (int i = 0; i < pointNum; ++i)
            {
                data[baseOffset + i] = ptr->Data[i] - 10 * (/*19 - ptr->gain*/ + 118.5); //将偏移量扩大10倍防止转换为整形的过程中丧失精度 由上位机最终还原
            }
            isFill[channel] = true;
        }
    }
    for (auto& f : isFill)
        if (!f)
            return false;
    return true;
}

int FFT_PACK::MakeProtocol(int pack_order)
{
    return fftPackData.MakeNetProtocol(data, pointNum, pack_order);
}

std::shared_ptr<FFT_PACK> FFT_PACK::DataProcessFFT(struct Struct_FFT* ptr)
{
    std::shared_ptr<FFT_PACK> fft_data = nullptr;
    int channel = ptr->bandNum;
    short key = ptr->identify;
    if (dict.contains(key))
    {
        if (channel >= 0 && channel < 57)
        {
            if (dict[key]->push(ptr))
            {
                fft_data = dict[key];
                dict.remove(key);
            }
        }
        else
            dict.remove(key);
    }
    else
    {
        if (channel >= 0 && channel < 57)
        {
            dict[key] = std::make_shared<FFT_PACK>(ptr);
        }
    }
    if (dict.size() > 500)
    {
        dict.clear();
    }
    return fft_data;
}
