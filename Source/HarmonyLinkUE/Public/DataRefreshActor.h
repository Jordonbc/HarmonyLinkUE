// DataRefreshActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HarmonyLinkClient.h"
#include "DataRefreshActor.generated.h"

UCLASS()
class HARMONYLINKUE_API ADataRefreshActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADataRefreshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	FServerStatusCallback ServerStatusCallback;

	// Function to refresh data
	UFUNCTION()
	void RefreshData();
	UFUNCTION()
	void OnServerStatusChecked(bool connected);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Timer interval for data refreshing (in seconds)
	UPROPERTY(EditAnywhere, Category = "Data Refresh")
	float RefreshInterval;

	// Timer handle for data refreshing
	FTimerHandle RefreshTimerHandle;
};
