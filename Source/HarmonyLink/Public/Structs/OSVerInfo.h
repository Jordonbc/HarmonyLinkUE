// Copyright (C) 2023  Jordon Brooks
#pragma once

#include "CoreMinimal.h"

#include "OSVerInfo.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FOSVerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Version = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString VersionID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString VersionCodename;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PrettyName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString VariantID;
};
