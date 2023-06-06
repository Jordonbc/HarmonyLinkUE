// .cpp file

#include "HarmonyLinkUEBPLibrary.h"
#include "Interfaces/IHttpResponse.h"

FAllInfo UHarmonyLinkUEBPLibrary::CachedAllInfo;
FHarmonyLinkVersionData UHarmonyLinkUEBPLibrary::CachedVersionData;
bool UHarmonyLinkUEBPLibrary::Connected;

UHarmonyLinkUEBPLibrary::UHarmonyLinkUEBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UHarmonyLinkUEBPLibrary::CheckServerStatus(FServerStatusCallback Callback)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:9000/are_you_there"));
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda(&UHarmonyLinkUEBPLibrary::OnCheckServerStatusResponseReceived, Callback);

	Request->ProcessRequest();
}

void UHarmonyLinkUEBPLibrary::RefreshOSInfo(FOSInfoCallback Callback)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:9000/os_info"));
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda(&UHarmonyLinkUEBPLibrary::OnOSInfoResponseReceived, Callback);

	Request->ProcessRequest();
}

void UHarmonyLinkUEBPLibrary::RefreshBatteryInfo(FBatteryInfoCallback Callback)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:9000/battery_info"));
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda(&UHarmonyLinkUEBPLibrary::OnBatteryInfoResponseReceived, Callback);

	Request->ProcessRequest();
}

void UHarmonyLinkUEBPLibrary::RefreshAllInfo(FDeviceInfoCallback Callback)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:9000/all_info"));
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda(&UHarmonyLinkUEBPLibrary::OnDeviceInfoResponseReceived, Callback);

	Request->ProcessRequest();
}

void UHarmonyLinkUEBPLibrary::RefreshVersionInfo(FVersionInfoCallback Callback)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:9000/version_info"));
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda(&UHarmonyLinkUEBPLibrary::OnVersionInfoResponseReceived, Callback);

	Request->ProcessRequest();
}

void UHarmonyLinkUEBPLibrary::OnCheckServerStatusResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FServerStatusCallback Callback)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		bool bConnected = Response->GetContentAsString().ToBool();

		Connected = bConnected;
		
		Callback.ExecuteIfBound(bConnected);
	}
	else
	{
		Connected = false;
		
		Callback.ExecuteIfBound(false);
		// The server returned an error, or the request failed.
		// Handle the error here.
	}
}

void UHarmonyLinkUEBPLibrary::OnOSInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOSInfoCallback Callback)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		FString ResponseString = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);

		// Parse the JSON response
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// Parse the OSInfo
			FOSInfo OSInfo;
			if (JsonObject->HasField("os_info"))
			{
				TSharedPtr<FJsonObject> OSInfoObject = JsonObject->GetObjectField("os_info");
				OSInfo.OSType = OSInfoObject->GetStringField("os_type");
				OSInfo.OSVersion = OSInfoObject->GetStringField("os_version");
				OSInfo.OSEdition = OSInfoObject->GetStringField("os_edition");
				OSInfo.OSBits = OSInfoObject->GetStringField("os_bits");
			}
			
			CachedAllInfo.OSInfo = OSInfo;
			Callback.ExecuteIfBound(OSInfo);
		}
	}
	else
	{
		// The server returned an error, or the request failed.
		// Handle the error here.
	}
}

void UHarmonyLinkUEBPLibrary::OnBatteryInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FBatteryInfoCallback Callback)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		FString ResponseString = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);

		// Parse the JSON response
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// Parse the BatteryInfo
			FBatteryInfo BatteryInfo;
			BatteryInfo.bHasBattery = JsonObject->GetBoolField("has_battery");
			BatteryInfo.BatteryPercent = JsonObject->GetIntegerField("battery_percent");
			FString ChargingStatusString = JsonObject->GetStringField("charging_status");
			if (ChargingStatusString == "charging")
			{
				BatteryInfo.ChargingStatus = EChargingStatus::Charging;
			}
			else if (ChargingStatusString == "battery")
			{
				BatteryInfo.ChargingStatus = EChargingStatus::Battery;
			}
			else
			{
				BatteryInfo.ChargingStatus = EChargingStatus::Unknown;
			}

			CachedAllInfo.BatteryInfo = BatteryInfo;

			Callback.ExecuteIfBound(BatteryInfo);
		}
	}
	else
	{
		// The server returned an error, or the request failed.
		// Handle the error here.
	}
}

void UHarmonyLinkUEBPLibrary::OnDeviceInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FDeviceInfoCallback Callback)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		FString ResponseString = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);

		// Parse the JSON response
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// Parse the OSInfo
			FOSInfo OSInfo;
			if (JsonObject->HasField("os_info"))
			{
				TSharedPtr<FJsonObject> OSInfoObject = JsonObject->GetObjectField("os_info");
				OSInfo.OSType = OSInfoObject->GetStringField("os_type");
				OSInfo.OSVersion = OSInfoObject->GetStringField("os_version");
				OSInfo.OSEdition = OSInfoObject->GetStringField("os_edition");
				OSInfo.OSBits = OSInfoObject->GetStringField("os_bits");
			}

			// Parse the Battery_Info
			FBatteryInfo BatteryInfo;
			const TSharedPtr<FJsonObject, ESPMode::ThreadSafe>* BatteryInfoObject = nullptr;
			if (JsonObject->HasField("dock_info") && JsonObject->TryGetObjectField("battery_info", BatteryInfoObject))
			{
				BatteryInfo.bHasBattery = BatteryInfoObject->Get()->GetBoolField("has_battery");
				BatteryInfo.BatteryPercent = BatteryInfoObject->Get()->GetIntegerField("battery_percent");

				FString battery = BatteryInfoObject->Get()->GetStringField("charging_status");
				if (battery.ToLower()  == "charging") { BatteryInfo.ChargingStatus = EChargingStatus::Charging; }
				else if (battery.ToLower()  == "battery") { BatteryInfo.ChargingStatus = EChargingStatus::Battery; }
				else { BatteryInfo.ChargingStatus = EChargingStatus::Unknown; }
				
			}

			// Parse the DockInfo
			FDockInfo DockInfo;
			const TSharedPtr<FJsonObject, ESPMode::ThreadSafe>* DockInfoObject = nullptr;
			if (JsonObject->HasField("dock_info") && JsonObject->TryGetObjectField("dock_info", DockInfoObject))
			{
				const TSharedPtr<FJsonObject, ESPMode::ThreadSafe>* DockModelObject = nullptr;
				if (DockInfoObject->Get()->HasField("dock_model") && JsonObject->TryGetObjectField("dock_model", DockModelObject))
				{
					TSharedPtr<FJsonObject> DockModelSubObject = DockModelObject->Get()->GetObjectField("dock_model");

					FString DockModel = DockModelObject->Get()->GetStringField("model");
					if (DockModel == "SteamDeckDock") { DockInfo.DockModel.Model = EDockModel::SteamDeckDock; }
					else if (DockModel == "JSAUX") { DockInfo.DockModel.Model = EDockModel::JSAUX; }
					else if (DockModel == "Unknown") { DockInfo.DockModel.Model = EDockModel::Unknown; }
					
					DockInfo.DockModel.Name = DockModelObject->Get()->GetStringField("name");
				}
				else
				{
					// handle case where dock_model is null
					DockInfo.DockModel.Model = EDockModel::Unknown;
					DockInfo.DockModel.Name = "Unknown";
				}

				DockInfo.bIsDocked = DockInfoObject->Get()->GetBoolField("is_docked");
				DockInfo.bFallbackDetection = DockInfoObject->Get()->GetBoolField("fallback_detection");
			}

			// Create the DeviceInfo struct
			FAllInfo AllInfo;
			AllInfo.OSInfo = OSInfo;
			AllInfo.DockInfo = DockInfo;
			AllInfo.BatteryInfo = BatteryInfo;

			CachedAllInfo = AllInfo;

			// Call the callback with the DeviceInfo
			Callback.ExecuteIfBound(AllInfo);

			// Handle the DeviceInfo struct as needed
			// ...
		}
	}
	else
	{
		// The server returned an error, or the request failed.
		// Handle the error here.
	}
}


void UHarmonyLinkUEBPLibrary::OnVersionInfoResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FVersionInfoCallback Callback)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		FString ResponseString = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);

		// Parse the JSON response
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FHarmonyLinkVersionData VersionInfo;
			VersionInfo.BuildTimestamp = JsonObject->GetStringField("build_timestamp");
			VersionInfo.GitBranch = JsonObject->GetStringField("git_branch");
			VersionInfo.GitDescribe = JsonObject->GetStringField("git_describe");
			VersionInfo.GitCommitTimestamp = JsonObject->GetStringField("git_commit_timestamp");
			VersionInfo.bDebug = JsonObject->GetBoolField("debug");
			VersionInfo.Version = JsonObject->GetStringField("version");
			VersionInfo.VersionMajor = JsonObject->GetIntegerField("version_major");
			VersionInfo.VersionMinor = JsonObject->GetIntegerField("version_minor");
			VersionInfo.VersionPatch = JsonObject->GetIntegerField("version_patch");
			VersionInfo.VersionPre = JsonObject->GetStringField("version_pre");

			CachedVersionData = VersionInfo;
			Callback.ExecuteIfBound(VersionInfo);
		}
	}
	else
	{
		// The server returned an error, or the request failed.
		// Handle the error here.
	}
}
