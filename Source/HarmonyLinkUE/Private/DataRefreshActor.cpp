// DataRefreshActor.cpp

#include "DataRefreshActor.h"
#include "HarmonyLinkUEBPLibrary.h"

// Sets default values
ADataRefreshActor::ADataRefreshActor()
{
	// Set default refresh interval to 1 second
	RefreshInterval = 1.0f;

	// Create a SceneComponent for the actor
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DataRefreshSceneComponent"));
	SetRootComponent(SceneComponent);
}

// Called when the game starts or when spawned
void ADataRefreshActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind the delegate
	ServerStatusCallback.BindUFunction(this, FName("OnServerStatusChecked"));

	// Start the timer for data refreshing
	GetWorldTimerManager().SetTimer(RefreshTimerHandle, this, &ADataRefreshActor::RefreshData, RefreshInterval, true);
}

void ADataRefreshActor::Destroyed()
{
	// Unbind the delegate
	ServerStatusCallback.Unbind();

	// Remember to call the parent class's Destroyed function at the end.
	Super::Destroyed();
}


// Called every frame
void ADataRefreshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}

void ADataRefreshActor::OnServerStatusChecked(bool connected)
{
	UE_LOG(LogTemp, Display, TEXT("Refreshing"));
	if (connected)
	{
		// Refresh the data and update the cached information
		UHarmonyLinkUEBPLibrary::RefreshAllInfo(FDeviceInfoCallback());
		UHarmonyLinkUEBPLibrary::RefreshVersionInfo(FVersionInfoCallback());
	}
}

// Function to refresh data
void ADataRefreshActor::RefreshData()
{
	// Only refresh data if the Harmony Link library is available
	if (UHarmonyLinkUEBPLibrary::StaticClass())
	{
		// Check server status asynchronously
		UHarmonyLinkUEBPLibrary::CheckServerStatus(ServerStatusCallback);
	}
}



