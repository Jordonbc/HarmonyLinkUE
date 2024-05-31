// Copyright (C) 2024 Jordon Brooks

#include "Structs/Device.h"

#include <Structs/FDevice.h>

FDevice::FDevice(HarmonyLinkLib::FDevice* oldDevice)
{
	if (oldDevice)
	{
		Device = Convert(oldDevice->device);
		Platform = Convert(oldDevice->platform);

		oldDevice->free();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert FDevice."))
	}
}

EDevice FDevice::Convert(HarmonyLinkLib::EDevice Device)
{
	switch (Device)
	{
	case HarmonyLinkLib::EDevice::DESKTOP:
		return EDevice::DESKTOP;
	case HarmonyLinkLib::EDevice::LAPTOP:
		return EDevice::LAPTOP;
	case HarmonyLinkLib::EDevice::HANDHELD:
		return EDevice::HANDHELD;
	case HarmonyLinkLib::EDevice::STEAM_DECK:
		return EDevice::STEAM_DECK;
	default:
		return EDevice::DESKTOP;
	}
}

EPlatform FDevice::Convert(HarmonyLinkLib::EPlatform Platform)
{
	switch (Platform)
	{
	case HarmonyLinkLib::EPlatform::WINDOWS:
		return EPlatform::WINDOWS;
	case HarmonyLinkLib::EPlatform::LINUX:
		return EPlatform::LINUX;
	case HarmonyLinkLib::EPlatform::MAC:
		return EPlatform::MAC;
	case HarmonyLinkLib::EPlatform::UNIX:
		return EPlatform::UNIX;
	default:
		return EPlatform::WINDOWS;
	}
}
