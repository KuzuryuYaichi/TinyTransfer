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
    if (lastPointNum != pointNum) //(��Ҫ)��¼����һ�ε�PACK_LEN �������ֱ��ʺ�������֮ǰ�ֱ��ʲ�һ���������һ�µ�����ʱ���µ�ƴ����ʾ����
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
                data[baseOffset + i] = ptr->Data[i] - 10 * (/*19 - ptr->gain*/ + 118.5); //��ƫ��������10����ֹת��Ϊ���εĹ�����ɥʧ���� ����λ�����ջ�ԭ
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
