// Copyright (C) 2024 Jordon Brooks

#pragma once

#include "CoreMinimal.h"
#include "HLConfigValue.generated.h"

UENUM(BlueprintType)
enum class EConfigValueType : uint8
{
	Int,
	Float,
	Bool,
	String
};

USTRUCT(BlueprintType)
struct FHLConfigValue
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="ConfigValue")
	EConfigValueType Type;

	UPROPERTY(EditAnywhere, Category="ConfigValue")
	int32 IntValue;

	UPROPERTY(EditAnywhere, Category="ConfigValue")
	float FloatValue;

	UPROPERTY(EditAnywhere, Category="ConfigValue")
	bool BoolValue;

	UPROPERTY(EditAnywhere, Category="ConfigValue")
	FString StringValue;

public:
	FHLConfigValue() : Type(EConfigValueType::String), IntValue(0), FloatValue(0.0f), BoolValue(false), StringValue(TEXT("")) {}

	FHLConfigValue(int32 Value) : Type(EConfigValueType::Int), IntValue(Value), FloatValue(0.0f), BoolValue(false), StringValue(TEXT("")) {}

	FHLConfigValue(float Value) : Type(EConfigValueType::Float), IntValue(0), FloatValue(Value), BoolValue(false), StringValue(TEXT("")) {}

	FHLConfigValue(bool Value) : Type(EConfigValueType::Bool), IntValue(0), FloatValue(0.0f), BoolValue(Value), StringValue(TEXT("")) {}

	FHLConfigValue(const FString& Value) : Type(EConfigValueType::String), IntValue(0), FloatValue(0.0f), BoolValue(false), StringValue(Value) {}

	EConfigValueType GetType() const
	{
		return Type;
	}

	template <typename T>
	T GetValue() const;
};

// Specializations of the templated getter
template<>
inline int32 FHLConfigValue::GetValue<int32>() const
{
	ensure(Type == EConfigValueType::Int);
	return IntValue;
}

template<>
inline float FHLConfigValue::GetValue<float>() const
{
	ensure(Type == EConfigValueType::Float);
	return FloatValue;
}

template<>
inline bool FHLConfigValue::GetValue<bool>() const
{
	ensure(Type == EConfigValueType::Bool);
	return BoolValue;
}

template<>
inline FString FHLConfigValue::GetValue<FString>() const
{
	ensure(Type == EConfigValueType::String);
	return StringValue;
}
