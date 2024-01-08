// Copyright (C) 2023  Jordon Brooks


#include "HarmonyLinkLibrary.h"

#include <HarmonyLinkLib.h>

bool UHarmonyLinkLibrary::IsWine()
{
	return HarmonyLinkLib::get_is_wine();
}

bool UHarmonyLinkLibrary::IsLinux()
{
#if PLATFORM_WINDOWS
	return IsWine();
#elif PLATFORM_LINUX
	return true;
#else
	return false;
#endif
}

bool UHarmonyLinkLibrary::IsSteamDeck()
{
	return GetDeviceInfo().Device == EDeviceEnum::STEAM_DECK;
}

FCPUInfo UHarmonyLinkLibrary::GetCPUInfo()
{
	return FCPUInfo(HarmonyLinkLib::get_cpu_info());
}

FDevice UHarmonyLinkLibrary::GetDeviceInfo()
{
	return FDevice(HarmonyLinkLib::get_device_info());
}

FOSVerInfo UHarmonyLinkLibrary::GetOSInfo()
{
	return FOSVerInfo(HarmonyLinkLib::get_os_version());
}

FBattery UHarmonyLinkLibrary::GetBatteryStatus()
{
	return FBattery(HarmonyLinkLib::get_battery_status());
}
