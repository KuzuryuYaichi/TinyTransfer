#include "pch.h"

#include "streaming_dma.h"

#pragma comment(lib, "setupapi.lib")

// ============= Static Utility Functions =====================================

std::vector<std::string> get_device_paths(GUID guid) {

    auto device_info = SetupDiGetClassDevs((LPGUID)&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (device_info == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetDevices INVALID_HANDLE_VALUE");
    }

    SP_DEVICE_INTERFACE_DATA device_interface = { 0 };
    device_interface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // enumerate through devices

    std::vector<std::string> device_paths;

    for (unsigned index = 0;
        SetupDiEnumDeviceInterfaces(device_info, NULL, &guid, index, &device_interface);
        ++index) {

        // get required buffer size
        unsigned long detailLength = 0;
        if (!SetupDiGetDeviceInterfaceDetail(device_info, &device_interface, NULL, 0, &detailLength, NULL) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            throw std::runtime_error("SetupDiGetDeviceInterfaceDetail - get length failed");
        }

        // allocate space for device interface detail
        auto dev_detail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(new char[detailLength]);
        dev_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // get device interface detail
        if (!SetupDiGetDeviceInterfaceDetail(device_info, &device_interface, dev_detail, detailLength, NULL, NULL)) {
            delete[] dev_detail;
            throw std::runtime_error("SetupDiGetDeviceInterfaceDetail - get detail failed");
        }
        device_paths.emplace_back(dev_detail->DevicePath);
        delete[] dev_detail;
    }

    SetupDiDestroyDeviceInfoList(device_info);

    return device_paths;
}
