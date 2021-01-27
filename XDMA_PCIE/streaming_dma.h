#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>
#include <SetupAPI.h>
#include <INITGUID.H>

#include "xdma_public.h"

// 74c7e4a9-6d5d-4a70-bc0d-20691dff9e9d
DEFINE_GUID(GUID_DEVINTERFACE_XDMA,
    0x74c7e4a9, 0x6d5d, 0x4a70, 0xbc, 0x0d, 0x20, 0x69, 0x1d, 0xff, 0x9e, 0x9d);

std::vector<std::string> get_device_paths(GUID guid);