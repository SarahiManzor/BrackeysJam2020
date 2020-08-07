#include "HandControllerBase.h"
#include "MotionControllerComponent.h" 
#include "Components/StaticMeshComponent.h"

AHandControllerBase::AHandControllerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(Root);

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ControllerMesh->SetupAttachment(MotionController);

	bCanTeleport = true;
}

// Called when the game starts or when spawned
void AHandControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHandControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AHandControllerBase::CanTeleport()
{
	return bCanTeleport;
}

void AHandControllerBase::SetTrackingSource(EControllerHand Hand)
{
	if (MotionController)
	{
		MotionController->SetTrackingSource(Hand);
	}
}

void AHandControllerBase::Grab()
{

}

void AHandControllerBase::Release()
{

}

void AHandControllerBase::ThumbStickY(float AxisValue)
{

}

void AHandControllerBase::TriggerPressed()
{

}

void AHandControllerBase::TriggerReleased()
{

}

void AHandControllerBase::ThumbstickClickPressed()
{

}

