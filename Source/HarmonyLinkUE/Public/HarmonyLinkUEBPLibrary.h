// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Engine/World.h"
#include "Engine/WorldComposition.h"

#include "HarmonyLinkUEBPLibrary.generated.h"

UENUM(BlueprintType)
enum class EDockModel : uint8
{
	SteamDeckDock,
	JSAUX,
	Unknown,
};

USTRUCT(BlueprintType)
struct FSDock
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDockModel Model = EDockModel::Unknown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "";

	void Print() const
	{
		UE_LOG(LogTemp, Display, TEXT("Dock Model: %s"), *StaticEnum<EDockModel>()->GetNameStringByValue(static_cast<int64>(Model)));
		UE_LOG(LogTemp, Display, TEXT("Name: %s"), *Name);
	}
};

USTRUCT(BlueprintType)
struct FDockInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSDock DockModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFallbackDetection = false;

	void Print() const
	{
		DockModel.Print();
		UE_LOG(LogTemp, Display, TEXT("IsDocked: %s"), bIsDocked ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Display, TEXT("FallbackDetection: %s"), bFallbackDetection ? TEXT("true") : TEXT("false"));
	}
};


USTRUCT(BlueprintType)
struct FOSInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OSType = "Unknown";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OSVersion = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OSEdition = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OSBits = "X64";

	void Print() const
	{
		UE_LOG(LogTemp, Display, TEXT("OS Type: %s"), *OSType);
		UE_LOG(LogTemp, Display, TEXT("OS Version: %s"), *OSVersion);
		UE_LOG(LogTemp, Display, TEXT("OS Edition: %s"), *OSEdition);
		UE_LOG(LogTemp, Display, TEXT("OS Bits: %s"), *OSBits);
	}
};

UENUM(BlueprintType)
enum class EChargingStatus : uint8
{
	Charging,
	Battery,
	Unknown
};

USTRUCT(BlueprintType)
struct FBatteryInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBattery = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BatteryPercent = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EChargingStatus ChargingStatus = EChargingStatus::Unknown;

	void Print() const
	{
		UE_LOG(LogTemp, Display, TEXT("Has Battery: %s"), bHasBattery ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Display, TEXT("Battery Percent: %d"), BatteryPercent);
		UE_LOG(LogTemp, Display, TEXT("Charging Status: %s"), *StaticEnum<EChargingStatus>()->GetNameStringByValue(static_cast<int64>(ChargingStatus)));
	}
};

USTRUCT(BlueprintType)
struct FHarmonyLinkVersionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BuildTimestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GitBranch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GitDescribe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GitCommitTimestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Version;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 VersionMajor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 VersionMinor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 VersionPatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VersionPre;

	void Print() const
	{
		UE_LOG(LogTemp, Display, TEXT("Build Timestamp: %s"), *BuildTimestamp);
		UE_LOG(LogTemp, Display, TEXT("Git Branch: %s"), *GitBranch);
		UE_LOG(LogTemp, Display, TEXT("Git Describe: %s"), *GitDescribe);
		UE_LOG(LogTemp, Display, TEXT("Git Commit Timestamp: %s"), *GitCommitTimestamp);
		UE_LOG(LogTemp, Display, TEXT("Debug: %s"), bDebug ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Display, TEXT("Version: %s"), *Version);
		UE_LOG(LogTemp, Display, TEXT("Version Major: %d"), VersionMajor);
		UE_LOG(LogTemp, Display, TEXT("Version Minor: %d"), VersionMinor);
		UE_LOG(LogTemp, Display, TEXT("Version Patch: %d"), VersionPatch);
		UE_LOG(LogTemp, Display, TEXT("Version Pre: %s"), *VersionPre);
	}
};

USTRUCT(BlueprintType)
struct FAllInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOSInfo OSInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDockInfo DockInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBatteryInfo BatteryInfo;

	void Print() const
	{
		UE_LOG(LogTemp, Display, TEXT("All Info:"));
		OSInfo.Print();
		DockInfo.Print();
		BatteryInfo.Print();
	}
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FDeviceInfoCallback, const FAllInfo&, DeviceInfo);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOSInfoCallback, const FOSInfo&, OSInfo);
DECLARE_DYNAMIC_DELEGATE_OneParam(FBatteryInfoCallback, const FBatteryInfo&, BatteryInfo);
DECLARE_DYNAMIC_DELEGATE_OneParam(FVersionInfoCallback, const FHarmonyLinkVersionData&, VersionInfo);
DECLARE_DYNAMIC_DELEGATE_OneParam(FServerStatusCallback, const bool, connected);


/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class HARMONYLINKUE_API UHarmonyLinkUEBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static bool Connected;

	// Cached DeviceInfo variable
	static FAllInfo CachedAllInfo;

	// Cached DeviceInfo variable
	static FHarmonyLinkVersionData CachedVersionData;

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static bool GetIsConnected()
	{ return Connected; }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static void PrintAllInfo()
	{ CachedAllInfo.Print(); }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static FAllInfo GetAllInfo()
	{ return CachedAllInfo; }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static FBatteryInfo GetBatteryInfo()
	{ return CachedAllInfo.BatteryInfo; }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static FDockInfo GetDockInfo()
	{ return CachedAllInfo.DockInfo; }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static FOSInfo GetOSInfo()
	{ return CachedAllInfo.OSInfo; }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static FHarmonyLinkVersionData GetVersionInfo()
	{ return CachedVersionData; }

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static void CheckServerStatus(FServerStatusCallback Callback);

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static void RefreshOSInfo(FOSInfoCallback Callback);

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static void RefreshBatteryInfo(FBatteryInfoCallback Callback);

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static void RefreshAllInfo(FDeviceInfoCallback Callback);

	UFUNCTION(BlueprintCallable, Category = "Harmony Link")
	static void RefreshVersionInfo(FVersionInfoCallback Callback);

private:
	static void OnCheckServerStatusResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FServerStatusCallback Callback);
	static void OnOSInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOSInfoCallback Callback);
	static void OnBatteryInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FBatteryInfoCallback Callback);
	static void OnDeviceInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FDeviceInfoCallback Callback);
	static void OnVersionInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FVersionInfoCallback Callback);
};
