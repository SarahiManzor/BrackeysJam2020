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

