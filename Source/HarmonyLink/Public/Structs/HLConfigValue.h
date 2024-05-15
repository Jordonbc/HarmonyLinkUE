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

	// Equality operators
	bool operator==(const FHLConfigValue& Other) const
	{
		if (Type != Other.Type)
		{
			return false;
		}

		switch (Type)
		{
		case EConfigValueType::Int:
			return IntValue == Other.IntValue;
		case EConfigValueType::Float:
			return FloatValue == Other.FloatValue;
		case EConfigValueType::Bool:
			return BoolValue == Other.BoolValue;
		case EConfigValueType::String:
			return StringValue == Other.StringValue;
		default:
			return false;
		}
	}

	bool operator!=(const FHLConfigValue& Other) const
	{
		return !(*this == Other);
	}
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

// Hash function
FORCEINLINE uint32 GetTypeHash(const FHLConfigValue& Value)
{
	uint32 Hash = GetTypeHash(Value.GetType());

	switch (Value.GetType())
	{
	case EConfigValueType::Int:
		Hash = HashCombine(Hash, GetTypeHash(Value.GetValue<int32>()));
		break;
	case EConfigValueType::Float:
		Hash = HashCombine(Hash, GetTypeHash(Value.GetValue<float>()));
		break;
	case EConfigValueType::Bool:
		Hash = HashCombine(Hash, GetTypeHash(Value.GetValue<bool>()));
		break;
	case EConfigValueType::String:
		Hash = HashCombine(Hash, GetTypeHash(Value.GetValue<FString>()));
		break;
	}

	return Hash;
}
