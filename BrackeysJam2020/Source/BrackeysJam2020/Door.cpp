#include "Door.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root);

	IsOpening = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	CloseLocation = GetActorLocation();

	// Convert from relative to world
	RelativeOpenLocation = GetTransform().TransformPosition(RelativeOpenLocation);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector TargetLocation = IsOpening ? RelativeOpenLocation : CloseLocation;
	FVector Direction = TargetLocation - GetActorLocation();
	float Distance = FMath::Min(Direction.Size(), 5.0f);
	Direction.Normalize();

	FVector NewLocation = GetActorLocation() + Direction  * Distance;
	SetActorLocation(NewLocation);
}

void ADoor::Open()
{
	IsOpening = true;
}

void ADoor::Close()
{
	IsOpening = false;
}

