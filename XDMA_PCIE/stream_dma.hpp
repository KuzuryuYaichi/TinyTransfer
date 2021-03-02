#pragma once

#include <string>
#include <thread>
#include <array>
#include <iostream>
#include <system_error>
#include <Windows.h>
#include <SetupAPI.h>

constexpr size_t array_size = 2048 / 4;

// ============ windows device handle ===========
struct device_file {
    HANDLE h;
    device_file();
    device_file(const std::string& path, DWORD accessFlags);
    ~device_file();
};

device_file::device_file()
{

}

device_file::device_file(const std::string& path, DWORD accessFlags) {
    h = CreateFile(path.c_str(), accessFlags, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("CreateFile control failed: " + std::to_string(GetLastError()));
    }
}

device_file::~device_file() {
    CloseHandle(h);
}

// ============ XDMA device ==============
class xdma_device {
public:
    xdma_device();
    xdma_device(const std::string& device_path);
    // transfer data from Host PC to FPGA Card using SGDMA engine
    size_t write_to_engine(void* buffer, size_t size);
    // transfer data from FPGA Card to Host PC using SGDMA engine
    size_t read_from_engine(void* buffer, size_t size, int channel);
	void write_user_register(long addr, uint32_t value);
	uint32_t read_user_register(long addr);
private:
    device_file control;
    device_file h2c[3];
    device_file c2h[3];
    device_file user;
    uint32_t read_control_register(long addr);
	std::string device_path;
};

inline static uint32_t bit(uint32_t n) {
    return (1 << n);
}

inline static bool is_bit_set(uint32_t x, uint32_t n) {
    return (x & bit(n)) == bit(n);
}

xdma_device::xdma_device()
{

}

xdma_device::xdma_device(const std::string& device_path) :
    control(device_path + "\\control", GENERIC_READ | GENERIC_WRITE),
    //user(device_path + "\\user", GENERIC_READ | GENERIC_WRITE),
    h2c{ device_file(), device_file(), device_file() },
    c2h{ device_file(device_path + "\\c2h_0", GENERIC_READ),
        device_file(device_path + "\\c2h_1", GENERIC_READ),
        device_file(device_path + "\\c2h_2", GENERIC_READ) },
	    device_path(device_path){

    if (!is_bit_set(read_control_register(0x0), 15) || !is_bit_set(read_control_register(0x1000), 15)) {
        throw std::runtime_error("XDMA engines h2c_0 and/or c2h_0 are not streaming engines!");
    }
}

uint32_t xdma_device::read_control_register(long addr) {
    uint32_t value = 0;
    size_t num_bytes_read;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(control.h, addr, NULL, FILE_BEGIN)) {
        throw std::runtime_error("SetFilePointer failed: " + std::to_string(GetLastError()));
    }
    if (!ReadFile(control.h, (LPVOID)&value, 4, (LPDWORD)&num_bytes_read, NULL)) {
        throw std::runtime_error("ReadFile failed:" + std::to_string(GetLastError()));
    }
    return value;
}

uint32_t xdma_device::read_user_register(long addr) {
    uint32_t value = 0;
    size_t num_bytes_read;
    //if (INVALID_SET_FILE_POINTER == SetFilePointer(user.h, addr, NULL, FILE_BEGIN)) {
        //throw std::runtime_error("SetFilePointer failed: " + std::to_string(GetLastError()));
    //}
    //if (!ReadFile(user.h, (LPVOID)&value, 4, (LPDWORD)&num_bytes_read, NULL)) {
        //throw std::runtime_error("ReadFile failed:" + std::to_string(GetLastError()));
    //}
    return value;
}

void xdma_device::write_user_register(long addr, uint32_t value) {
    size_t num_bytes_read;
    //if (INVALID_SET_FILE_POINTER == SetFilePointer(user.h, addr, NULL, FILE_BEGIN)) {
        //throw std::runtime_error("SetFilePointer failed: " + std::to_string(GetLastError()));
    //}
    //if (!WriteFile(user.h, (LPVOID)&value, 4, (LPDWORD)&num_bytes_read, NULL)) {
        //throw std::runtime_error("ReadFile failed:" + std::to_string(GetLastError()));
    //}
}

size_t xdma_device::write_to_engine(void* buffer, size_t size) {
    unsigned long num_bytes_written;
    if (!WriteFile(h2c[0].h, buffer, (DWORD)size, &num_bytes_written, NULL)) {
        throw std::runtime_error("Failed to write to stream! " + std::to_string(GetLastError()));
    }
    return num_bytes_written;
}

size_t xdma_device::read_from_engine(void* buffer, size_t size, int channel) {
    unsigned long num_bytes_read;
    if (!ReadFile(c2h[channel].h, buffer, (DWORD)size, &num_bytes_read, NULL)) {
        throw std::runtime_error("Failed to read from stream! " + std::to_string(GetLastError()));
    }
    return num_bytes_read;
}

bool isRunning = true;

void SetIsRunning(bool state)
{
	isRunning = state;
}

struct paramStruct
{
    xdma_device* pDev;
    PDATA_CALLBACK* CallBack;
    int channel;
    size_t* TransferByte;
    size_t PACK_LEN;
    int PACK_NUM;

    paramStruct(){}

    paramStruct(xdma_device& dev, PDATA_CALLBACK& CallBack, size_t& TransferByte, int channel, size_t PACK_LEN, int PACK_NUM)
    {
        pDev = &dev;
        this->CallBack = &CallBack;
        this->channel = channel;
        this->TransferByte = &TransferByte;
        this->PACK_LEN = PACK_LEN;
        this->PACK_NUM = PACK_NUM;
    }
};

unsigned int pack_resolution = 3; // 0:512 1:1024 2:2048 3:4096 4:256
const unsigned int PACK_LEN_PER_PACK[] = { 400 * 2 + 10 * 8, 800 * 2 + 10 * 8, 1600 * 2 + 10 * 8, 3200 * 2 + 10 * 8, 200 * 2 + 10 * 8 };
const unsigned int PACK_NUM_PER_PACK[] = { 8, 4, 2, 1, 16 };
const unsigned int BLOCK_LEN_PER_PACK[] = { PACK_LEN_PER_PACK[0] * PACK_NUM_PER_PACK[0],
                                            PACK_LEN_PER_PACK[1] * PACK_NUM_PER_PACK[1],
                                            PACK_LEN_PER_PACK[2] * PACK_NUM_PER_PACK[2],
                                            PACK_LEN_PER_PACK[3] * PACK_NUM_PER_PACK[3],
                                            PACK_LEN_PER_PACK[4] * PACK_NUM_PER_PACK[4] };

void SetPackLenFFT(int resolution)
{
    pack_resolution = resolution;
}

void read_FFT(struct paramStruct* param)
{
    xdma_device& dev = *param->pDev;
    PDATA_CALLBACK& CallBack = *param->CallBack;
    int channel = param->channel;
    size_t& TransferByte = *param->TransferByte;
    const int PACK_NUM = param->PACK_NUM;
    const int PACK_LEN = param->PACK_LEN;
    const size_t BLOCK_LEN = PACK_LEN * PACK_NUM;

    char* buffer = new char[BLOCK_LEN];
    while (isRunning)
    {
        int pack_resolution_t = pack_resolution;
        size_t BLOCK_LEN_FFT = BLOCK_LEN_PER_PACK[pack_resolution_t] * PACK_NUM;
        size_t bytes_remaining = BLOCK_LEN_FFT;
        try {
            while (bytes_remaining > 0) {
                size_t offset = BLOCK_LEN_FFT - bytes_remaining;
                int read_len = dev.read_from_engine((char*)buffer + offset, bytes_remaining, channel);
                bytes_remaining -= read_len;
                TransferByte += read_len;
            }

            if (CallBack != nullptr)
                CallBack(buffer, PACK_LEN_PER_PACK[pack_resolution_t], PACK_NUM_PER_PACK[pack_resolution_t] * PACK_NUM);
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    delete[] buffer;
}

void read_NB(struct paramStruct* param)
{
    xdma_device& dev = *param->pDev;
    PDATA_CALLBACK& CallBack = *param->CallBack;
    int channel = param->channel;
    size_t& TransferByte = *param->TransferByte;
    const int PACK_NUM = param->PACK_NUM;
    const int PACK_LEN = param->PACK_LEN;
    const size_t BLOCK_LEN = PACK_LEN * PACK_NUM;

    char* buffer = new char[BLOCK_LEN];
    while (isRunning)
    {
		size_t bytes_remaining = BLOCK_LEN;
        try {
            while (bytes_remaining > 0) {
                size_t offset = BLOCK_LEN - bytes_remaining;
                int read_len = dev.read_from_engine((char*)buffer + offset, bytes_remaining, channel);
                bytes_remaining -= read_len;
                TransferByte += read_len;
            }

            //static unsigned int last = 0, now = 0;
            //now = *(unsigned int*)buffer;
            //if (now != last + 32)
            //    std::cout << "WBNB Error Order Now: " << now << " Last: " << last << std::endl;
            //last = now;

            if (CallBack != nullptr)
                CallBack(buffer, PACK_LEN, PACK_NUM);
        }
        catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
        }
    }
    delete[] buffer;
}

void read_WB(struct paramStruct* param)
{
    xdma_device& dev = *param->pDev;
    PDATA_CALLBACK& CallBack = *param->CallBack;
    int channel = param->channel;
    size_t& TransferByte = *param->TransferByte;
    const int PACK_NUM = param->PACK_NUM;
    const int PACK_LEN = param->PACK_LEN;
    const size_t BLOCK_LEN = (10 * 8 + PACK_LEN) * PACK_NUM;

    char* buffer = new char[BLOCK_LEN];
    while (isRunning)
    {
        size_t bytes_remaining = BLOCK_LEN;
        try {
            while (bytes_remaining > 0) {
                size_t offset = BLOCK_LEN - bytes_remaining;
                int read_len = dev.read_from_engine((char*)buffer + offset, bytes_remaining, channel);
                bytes_remaining -= read_len;
                TransferByte += read_len;
            }

            //static unsigned int last = 0, now = 0;
            //now = *(unsigned int*)buffer;
            //if (now != last + 64)
            //    std::cout << "WBNB Error Order Now: " << now << " Last: " << last << std::endl;
            //last = now;

            if (CallBack != nullptr)
                CallBack(buffer, PACK_LEN, PACK_NUM);
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    delete[] buffer;
}

DWORD WINAPI ThreadProc_NB(LPVOID lpParam)
{
	read_NB((struct paramStruct*)lpParam);
	return 0;
}

DWORD WINAPI ThreadProc_WB(LPVOID lpParam)
{
    read_WB((struct paramStruct*)lpParam);
    return 0;
}

DWORD WINAPI ThreadProc_FFT(LPVOID lpParam)
{
    read_FFT((struct paramStruct*)lpParam);
    return 0;
}

struct paramStruct Info[3];

size_t TransferByte_Nb, TransferByte_Wb, TransferByte_FFT;

void ReadThread(xdma_device& dev, PDATA_CALLBACK& CallBackNB, PDATA_CALLBACK& CallBackWB, PDATA_CALLBACK& CallBackFFT)
{
    const size_t PACK_LEN_NB = 39 * 8 * 32, PACK_LEN_WB = 5 * 8 * 64, PACK_LEN_FFT = 1680 * 8; //1680 * 8;
    Info[0] = paramStruct(dev, CallBackNB, TransferByte_Nb, 0, PACK_LEN_NB, 80);
    Info[1] = paramStruct(dev, CallBackFFT, TransferByte_FFT, 1, PACK_LEN_FFT, 80);
    Info[2] = paramStruct(dev, CallBackWB, TransferByte_Wb, 2, PACK_LEN_WB, 80);
    try {
		DWORD dwThreadID;
		CreateThread(NULL, 0, ThreadProc_NB, &Info[0], 0, &dwThreadID);
        CreateThread(NULL, 0, ThreadProc_FFT, &Info[1], 0, &dwThreadID);
        //CreateThread(NULL, 0, ThreadProc_WB, &Info[2], 0, &dwThreadID);
    }
    catch (const std::exception& e) {

    }
}
