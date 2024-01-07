// Copyright (C) 2023  Jordon Brooks

#include "Structs/Battery.h"

#include <Structs/FBattery.h>

FBattery::FBattery(HarmonyLinkLib::FBattery* battery)
{
	if (battery)
	{
		HasBattery = battery->has_battery;
		IsACConnected = battery->is_connected_to_ac;
		BatteryPercent = battery->battery_percent;

		battery->free();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert FBattery."))
	}
}
