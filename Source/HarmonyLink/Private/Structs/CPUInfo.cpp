// Copyright (C) 2024 Jordon Brooks

#include "Structs/CPUInfo.h"

FCPUInfo::FCPUInfo(HarmonyLinkLib::FCPUInfo* cpu_info)
{
	if (cpu_info)
	{
		VendorID = cpu_info->VendorID.c_str();
		ModelName = cpu_info->Model_Name.c_str();
		PhysicalCores = cpu_info->Physical_Cores;
		LogicalCores = cpu_info->Logical_Cores;

		for (const HarmonyLinkLib::FString& Flag : cpu_info->Flags)
		{
			Flags.Add(Flag.c_str());
		}

		cpu_info->free();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert FCPUInfo."))
	}
}
