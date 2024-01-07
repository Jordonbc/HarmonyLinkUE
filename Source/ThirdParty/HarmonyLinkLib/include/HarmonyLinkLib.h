// Copyright (C) 2023  Jordon Brooks

#pragma once

#include "Core.h"
#include "Structs/FBattery.h"
#include "Structs/FCPUInfo.h"
#include "Structs/FDevice.h"
#include "Structs/FOSVerInfo.h"


class IPlatformUtilities;

namespace HarmonyLinkLib
{
    extern "C" HARMONYLINKLIB_API bool get_is_wine();

    extern "C" HARMONYLINKLIB_API FCPUInfo* get_cpu_info();

    extern "C" HARMONYLINKLIB_API FDevice* get_device_info();

    extern "C" HARMONYLINKLIB_API FOSVerInfo* get_os_version();
    
    extern "C" HARMONYLINKLIB_API FBattery* get_battery_status();
}
