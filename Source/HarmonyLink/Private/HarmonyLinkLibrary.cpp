// Copyright (C) 2024 Jordon Brooks

#include "HarmonyLinkLibrary.h"
#include "HarmonyLink.h"

#include "HarmonyLinkLib.h"

UHarmonyLinkLibrary::UHarmonyLinkLibrary()
{
	if (!HarmonyLinkLib::HL_Init())
	{
		UE_LOG(LogHarmonyLink, Fatal, TEXT("Failed to initialise HarmonyLinkLib!"));
		return;
	}

	UE_LOG(LogHarmonyLink, Log, TEXT("HarmonyLinkLib Initialised!"));
}

bool UHarmonyLinkLibrary::IsWine()
{
	return HarmonyLinkLib::get_is_wine();
}

bool UHarmonyLinkLibrary::IsLinux()
{
	return HarmonyLinkLib::get_is_linux();
}

bool UHarmonyLinkLibrary::IsSteamDeck()
{
	return GetDeviceInfo().Device == EDevice::STEAM_DECK;
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
