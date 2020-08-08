#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "StateTracker.h"

// Sets default values
ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
/*
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
*/
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);

	IsOpening = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	CloseLocation = GetActorLocation();

	// Convert from relative to world
	RelativeOpenLocation = GetTransform().TransformPosition(RelativeOpenLocation);

	Tracker = Cast<UStateTracker>(GetComponentByClass(UStateTracker::StaticClass()));
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Tracker || !Tracker->IsRewinding())
	{
		if (IsOpening)
		{
			UE_LOG(LogTemp, Warning, TEXT("Opening"));
		}
		FVector TargetLocation = IsOpening ? RelativeOpenLocation : CloseLocation;
		FVector Direction = TargetLocation - GetActorLocation();
		float Distance = FMath::Min(Direction.Size(), 5.0f);
		Direction.Normalize();

		FVector NewLocation = GetActorLocation() + Direction * Distance;
		SetActorLocation(NewLocation, false, (FHitResult*)nullptr, ETeleportType::ResetPhysics);
	}
}

void ADoor::Open()
{
	UE_LOG(LogTemp, Warning, TEXT("Open"));
	IsOpening = true;
}

void ADoor::Close()
{
	IsOpening = false;
}

