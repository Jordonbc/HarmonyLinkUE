// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Structs/HLConfigValue.h"

/**
 * 
 */
class FHarmonyLinkSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	void CustomizeSettings(IDetailLayoutBuilder& DetailBuilder);

	static TArray<TSharedPtr<FString>>* GetComboBoxOptions();
	static EConfigValueType GetConfigValueTypeFromString(const FString& TypeString);
	static FString GetConfigValueTypeAsString(EConfigValueType Type);

	static TArray<TSharedPtr<FString>> ComboBoxOptions; // Static member for options
};
