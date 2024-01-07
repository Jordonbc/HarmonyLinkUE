// Copyright (C) 2023  Jordon Brooks


#include "HarmonyLinkLibrary.h"

#include <HarmonyLinkLib.h>

bool UHarmonyLinkLibrary::IsWine()
{
	static bool IsWine_ = HarmonyLinkLib::get_is_wine();
	return IsWine_;
}

bool UHarmonyLinkLibrary::IsLinux()
{
	static bool IsLinux_ =
#if PLATFORM_WINDOWS
		IsWine();
#elif PLATFORM_LINUX
		true;
#else
	false;
#endif
	
	return IsLinux_;
}

bool UHarmonyLinkLibrary::IsSteamDeck()
{
	static bool IsSteamDeck_ = GetDeviceInfo().Device == EDevice::STEAM_DECK;
	return IsSteamDeck_;
}

FCPUInfo UHarmonyLinkLibrary::GetCPUInfo()
{
	static FCPUInfo CPUInfo(HarmonyLinkLib::get_cpu_info());
	return CPUInfo;
}

FDevice UHarmonyLinkLibrary::GetDeviceInfo()
{
	return FDevice();
}

FOSVerInfo UHarmonyLinkLibrary::GetOSInfo()
{
	return FOSVerInfo();
}

FBattery UHarmonyLinkLibrary::GetBatteryStatus()
{
	return FBattery(HarmonyLinkLib::get_battery_status());
}
