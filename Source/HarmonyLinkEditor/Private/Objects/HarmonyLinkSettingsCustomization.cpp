// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/HarmonyLinkSettingsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Objects/HarmonyLinkGraphics.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SButton.h"

TArray<TSharedPtr<FString>> FHarmonyLinkSettingsCustomization::ComboBoxOptions;

TSharedRef<IDetailCustomization> FHarmonyLinkSettingsCustomization::MakeInstance()
{
    return MakeShareable(new FHarmonyLinkSettingsCustomization);
}

void FHarmonyLinkSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    CustomizeSettings(DetailBuilder);
}

void FHarmonyLinkSettingsCustomization::CustomizeSettings(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("HarmonyLink Settings");

    // Access the HarmonyLinkGraphics singleton instance
    UHarmonyLinkGraphics* HarmonyLinkGraphics = UHarmonyLinkGraphics::GetSettings();
    if (!HarmonyLinkGraphics)
    {
        return;
    }

    // Display Automatic Profile Switch
    Category.AddCustomRow(FText::FromString("Automatic Profile Switch"))
    .NameContent()
    [
        SNew(STextBlock)
        .Text(FText::FromString("Automatic Profile Switch"))
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked_Lambda([HarmonyLinkGraphics]() -> ECheckBoxState
        {
            return HarmonyLinkGraphics->GetAutomaticSwitching() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
        })
        .OnCheckStateChanged_Lambda([HarmonyLinkGraphics](ECheckBoxState NewState)
        {
            HarmonyLinkGraphics->SetAutomaticSwitching(NewState == ECheckBoxState::Checked);
        })
    ];

    // Iterate over profiles and display their settings
    const TMap<EProfile, FSettingsProfile>& Profiles = HarmonyLinkGraphics->GetProfiles();
    for (const auto& ProfilePair : Profiles)
    {
        const EProfile ProfileEnum = ProfilePair.Key;
        const FSettingsProfile& Profile = ProfilePair.Value;

        Category.AddCustomRow(FText::FromString(Profile.SectionName.ToString()))
        .NameContent()
        [
            SNew(STextBlock)
            .Text(FText::FromString(Profile.SectionName.ToString()))
        ];

        // Create a box to contain the table
        TSharedRef<SVerticalBox> Table = SNew(SVerticalBox);

        // Add table header
        Table->AddSlot()
        .AutoHeight()
        .Padding(2)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(2)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Name"))
            ]
            + SHorizontalBox::Slot()
            .Padding(2)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Value"))
            ]
            + SHorizontalBox::Slot()
            .Padding(2)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Type"))
            ]
        ];

        // Add rows for each setting in the profile
        for (const auto& SettingPair : Profile.Settings)
        {
            const FName& SettingName = SettingPair.Key;
            const FHLConfigValue& SettingValue = SettingPair.Value;

            TSharedPtr<SEditableTextBox> NameTextBox;
            TSharedPtr<SWidget> ValueWidget;
            TSharedPtr<SComboBox<TSharedPtr<FString>>> TypeComboBox;

            // Create the value widget based on the type
            switch (SettingValue.GetType())
            {
            case EConfigValueType::Int:
                ValueWidget = SNew(SSpinBox<int32>)
                .Value(SettingValue.GetValue<int32>())
                .OnValueCommitted_Lambda([HarmonyLinkGraphics, ProfileEnum, SettingName](int32 NewValue, ETextCommit::Type CommitType)
                {
                    HarmonyLinkGraphics->SetSetting(ProfileEnum, SettingName, FHLConfigValue(NewValue));
                });
                break;
            case EConfigValueType::Float:
                ValueWidget = SNew(SSpinBox<float>)
                .Value(SettingValue.GetValue<float>())
                .OnValueCommitted_Lambda([HarmonyLinkGraphics, ProfileEnum, SettingName](float NewValue, ETextCommit::Type CommitType)
                {
                    HarmonyLinkGraphics->SetSetting(ProfileEnum, SettingName, FHLConfigValue(NewValue));
                });
                break;
            case EConfigValueType::Bool:
                ValueWidget = SNew(SCheckBox)
                .IsChecked(SettingValue.GetValue<bool>() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                .OnCheckStateChanged_Lambda([HarmonyLinkGraphics, ProfileEnum, SettingName](ECheckBoxState NewState)
                {
                    bool NewValue = NewState == ECheckBoxState::Checked;
                    HarmonyLinkGraphics->SetSetting(ProfileEnum, SettingName, FHLConfigValue(NewValue));
                });
                break;
            case EConfigValueType::String:
                ValueWidget = SNew(SEditableTextBox)
                .Text(FText::FromString(SettingValue.GetValue<FString>()))
                .OnTextCommitted_Lambda([HarmonyLinkGraphics, ProfileEnum, SettingName](const FText& NewText, ETextCommit::Type CommitType)
                {
                    FString NewValue = NewText.ToString();
                    HarmonyLinkGraphics->SetSetting(ProfileEnum, SettingName, FHLConfigValue(NewValue));
                });
                break;
            }

            // Add the row for the setting
            Table->AddSlot()
            .AutoHeight()
            .Padding(2)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding(2)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(NameTextBox, SEditableTextBox)
                    .Text(FText::FromString(SettingName.ToString()))
                    .OnTextCommitted_Lambda([HarmonyLinkGraphics, ProfileEnum, SettingName](const FText& NewText, ETextCommit::Type CommitType)
                    {
                        FString NewName = NewText.ToString();
                        HarmonyLinkGraphics->RenameSetting(ProfileEnum, SettingName, FName(*NewName));
                    })
                ]
                + SHorizontalBox::Slot()
                .Padding(2)
                .VAlign(VAlign_Center)
                [
                    ValueWidget.ToSharedRef()
                ]
                + SHorizontalBox::Slot()
                .Padding(2)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(TypeComboBox, SComboBox<TSharedPtr<FString>>)
                    .OptionsSource(GetComboBoxOptions()) // Use static member
                    .OnGenerateWidget_Lambda([](TSharedPtr<FString> InOption)
                    {
                        return SNew(STextBlock).Text(FText::FromString(*InOption));
                    })
                    .OnSelectionChanged_Lambda([this, HarmonyLinkGraphics, ProfileEnum, SettingName](TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
                    {
                        if (NewSelection.IsValid())
                        {
                            EConfigValueType NewType = this->GetConfigValueTypeFromString(*NewSelection);
                            HarmonyLinkGraphics->ChangeSettingType(ProfileEnum, SettingName, NewType);
                        }
                    })
                    .Content()
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(GetConfigValueTypeAsString(SettingValue.GetType())))
                    ]
                ]
            ];
        }

        Category.AddCustomRow(FText::FromString(Profile.SectionName.ToString() + " Table"))
        [
            Table
        ];
    }
}

TArray<TSharedPtr<FString>>* FHarmonyLinkSettingsCustomization::GetComboBoxOptions()
{
    if (ComboBoxOptions.Num() == 0) // Initialize only once
    {
        ComboBoxOptions.Add(MakeShareable(new FString("int")));
        ComboBoxOptions.Add(MakeShareable(new FString("float")));
        ComboBoxOptions.Add(MakeShareable(new FString("bool")));
        ComboBoxOptions.Add(MakeShareable(new FString("string")));
    }
    return &ComboBoxOptions;
}

EConfigValueType FHarmonyLinkSettingsCustomization::GetConfigValueTypeFromString(const FString& TypeString)
{
    if (TypeString == "int")
    {
        return EConfigValueType::Int;
    }
    
    if (TypeString == "float")
    {
        return EConfigValueType::Float;
    }
    
    if (TypeString == "bool")
    {
        return EConfigValueType::Bool;
    }
    
    if (TypeString == "string")
    {
        return EConfigValueType::String;
    }
    
    return EConfigValueType::None;
}

FString FHarmonyLinkSettingsCustomization::GetConfigValueTypeAsString(EConfigValueType Type)
{
    switch (Type)
    {
    case EConfigValueType::Int:
        return "int";
    case EConfigValueType::Float:
        return "float";
    case EConfigValueType::Bool:
        return "bool";
    case EConfigValueType::String:
        return "string";
    default:
        return "none";
    }
}
