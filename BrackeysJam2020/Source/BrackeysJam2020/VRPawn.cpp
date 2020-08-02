#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "HandControllerBase.h"
#include "Components/InputComponent.h" 

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	if (RightHandControllerClass)
	{
		RightHandController = GetWorld()->SpawnActor<AHandControllerBase>(RightHandControllerClass);
		RightHandController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		RightHandController->SetOwner(this);
		RightHandController->SetTrackingSource(EControllerHand::Right);
	}
	if (LeftHandControllerClass)
	{
		LeftHandController = GetWorld()->SpawnActor<AHandControllerBase>(LeftHandControllerClass);
		LeftHandController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		LeftHandController->SetOwner(this);
		LeftHandController->SetTrackingSource(EControllerHand::Left);
	}
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("LeftGrab"), IE_Pressed, this, &AVRPawn::GrabButtonPressedLeft);
	PlayerInputComponent->BindAction(TEXT("LeftGrab"), IE_Released, this, &AVRPawn::GrabButtonReleasedLeft);

	PlayerInputComponent->BindAction(TEXT("RightGrab"), IE_Pressed, this, &AVRPawn::GrabButtonPressedRight);
	PlayerInputComponent->BindAction(TEXT("RightGrab"), IE_Released, this, &AVRPawn::GrabButtonReleasedRight);
}

void AVRPawn::GrabButtonPressedLeft()
{
	LeftHandController->Grab();
}

void AVRPawn::GrabButtonReleasedLeft()
{
	LeftHandController->Release();
}

void AVRPawn::GrabButtonPressedRight()
{
	RightHandController->Grab();
}

void AVRPawn::GrabButtonReleasedRight()
{
	RightHandController->Release();
}
