// Copyright (C) 2024 Jordon Brooks

#include "HarmonyLinkLibrary.h"
#include "HarmonyLink.h"

#include "HarmonyLinkLib.h"

bool UHarmonyLinkLibrary::bIsWineCached = false;
bool UHarmonyLinkLibrary::bIsLinuxCached = false;
bool UHarmonyLinkLibrary::bIsSteamDeckCached = false;
bool UHarmonyLinkLibrary::bCPUInfoCached = false;
bool UHarmonyLinkLibrary::bDeviceInfoCached = false;
bool UHarmonyLinkLibrary::bOSInfoCached = false;

bool UHarmonyLinkLibrary::bIsWine = false;
bool UHarmonyLinkLibrary::bIsLinux = false;
bool UHarmonyLinkLibrary::bIsSteamDeck = false;

FCPUInfo UHarmonyLinkLibrary::CachedCPUInfo = FCPUInfo();
FDevice UHarmonyLinkLibrary::CachedDeviceInfo = FDevice();
FOSVerInfo UHarmonyLinkLibrary::CachedOSInfo = FOSVerInfo();

UHarmonyLinkLibrary::UHarmonyLinkLibrary()
{
	if (!HarmonyLinkLib::HL_Init())
	{
		UE_LOG(LogHarmonyLink, Fatal, TEXT("Failed to initialise HarmonyLinkLib!"));
		return;
	}

	UE_LOG(LogHarmonyLink, Log, TEXT("HarmonyLinkLib Initialised!"));
}

bool UHarmonyLinkLibrary::IsWine(bool bForce)
{
	if (!bIsWineCached || bForce)
	{
		bIsWine = HarmonyLinkLib::get_is_wine();
		bIsWineCached = true;
	}
	return bIsWine;
}

bool UHarmonyLinkLibrary::IsLinux(bool bForce)
{
	if (!bIsLinuxCached || bForce)
	{
		bIsLinux = HarmonyLinkLib::get_is_linux();
		bIsLinuxCached = true;
	}
	return bIsLinux;
}

bool UHarmonyLinkLibrary::IsSteamDeck(bool bForce)
{
	if (!bIsSteamDeckCached || bForce)
	{
		bIsSteamDeck = GetDeviceInfo().Device == EDevice::STEAM_DECK;
		bIsSteamDeckCached = true;
	}
	return bIsSteamDeck;
}

FCPUInfo UHarmonyLinkLibrary::GetCPUInfo(bool bForce)
{
	if (!bCPUInfoCached || bForce)
	{
		CachedCPUInfo = FCPUInfo(HarmonyLinkLib::get_cpu_info());
		bCPUInfoCached = true;
	}

	return CachedCPUInfo;
}

FDevice UHarmonyLinkLibrary::GetDeviceInfo(bool bForce)
{
	if (!bDeviceInfoCached || bForce)
	{
		CachedDeviceInfo = FDevice(HarmonyLinkLib::get_device_info());
		bDeviceInfoCached = true;
	}
	return CachedDeviceInfo;
}

FOSVerInfo UHarmonyLinkLibrary::GetOSInfo(bool bForce)
{
	if (!bOSInfoCached || bForce)
	{
		CachedOSInfo = FOSVerInfo(HarmonyLinkLib::get_os_version());
		bOSInfoCached = true;
	}
	return CachedOSInfo;
}

FBattery UHarmonyLinkLibrary::GetBatteryStatus()
{
	return FBattery(HarmonyLinkLib::get_battery_status());
}
