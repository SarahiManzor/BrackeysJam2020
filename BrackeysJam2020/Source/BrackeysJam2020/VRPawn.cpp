#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "HandControllerBase.h"
#include "Components/InputComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Root);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(Root);

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(Root);

	TeleportingHand = nullptr;
	bSelectingTeleport = false;
	TeleportRangeVelocity = 500.0f;
	PlayerHeight = 0.9f;
	TeleportThreshold = 0.1f;
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	DestinationMarker->SetVisibility(false);

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

	// Account for camera location not centered at actual vr pawn location
	FVector CameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	CameraOffset.Z = 0.0f;
	SetActorLocation(GetActorLocation() + CameraOffset);
	Root->AddWorldOffset(-CameraOffset);

	if (bSelectingTeleport)
	{
		UpdateTeleportMarker();
	}
}

#pragma region Controller Inputs

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("LeftGrab"), IE_Pressed, this, &AVRPawn::GrabButtonPressedLeft);
	PlayerInputComponent->BindAction(TEXT("LeftGrab"), IE_Released, this, &AVRPawn::GrabButtonReleasedLeft);

	PlayerInputComponent->BindAction(TEXT("RightGrab"), IE_Pressed, this, &AVRPawn::GrabButtonPressedRight);
	PlayerInputComponent->BindAction(TEXT("RightGrab"), IE_Released, this, &AVRPawn::GrabButtonReleasedRight);

	PlayerInputComponent->BindAxis(TEXT("ThumbStickLeftY"), this, &AVRPawn::ThumbStickLeftY);
	PlayerInputComponent->BindAxis(TEXT("ThumbStickRightY"), this, &AVRPawn::ThumbStickRightY);

	PlayerInputComponent->BindAction(TEXT("LeftRewind"), IE_Pressed, this, &AVRPawn::RewindButtonPressedLeft);
	PlayerInputComponent->BindAction(TEXT("LeftRewind"), IE_Released, this, &AVRPawn::RewindButtonReleasedLeft);

	PlayerInputComponent->BindAction(TEXT("RightRewind"), IE_Pressed, this, &AVRPawn::RewindButtonPressedRight);
	PlayerInputComponent->BindAction(TEXT("RightRewind"), IE_Released, this, &AVRPawn::RewindButtonReleasedRight);

	PlayerInputComponent->BindAction(TEXT("LeftRotate"), IE_Pressed, this, &AVRPawn::RotateButtonPressedLeft);
	PlayerInputComponent->BindAction(TEXT("RightRotate"), IE_Pressed, this, &AVRPawn::RotateButtonPressedRight);

	PlayerInputComponent->BindAction(TEXT("LeftThumbstickClick"), IE_Pressed, this, &AVRPawn::ThumbstickClickPressedLeft);
	PlayerInputComponent->BindAction(TEXT("RightThumbstickClick"), IE_Pressed, this, &AVRPawn::ThumbstickClickPressedRight);


	UE_LOG(LogTemp, Warning, TEXT("Inputs Initialized"));
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

void AVRPawn::ThumbStickLeftY(float AxisValue)
{
	ManageTeleport(LeftHandController, AxisValue);
}

void AVRPawn::ThumbStickRightY(float AxisValue)
{
	ManageTeleport(RightHandController, AxisValue);
}

void AVRPawn::ManageTeleport(AHandControllerBase* HandController, float AxisValue)
{
	bool bIsTeleporting = TeleportingHand == HandController;
	if (!HandController->CanTeleport() && !bIsTeleporting)
	{
		HandController->ThumbStickY(AxisValue);
		return;
	}
	if (AxisValue > TeleportThreshold)
	{
		UE_LOG(LogTemp, Warning, TEXT("Teleport Pressed Right"));
		TeleportingHand = HandController;
		bSelectingTeleport = true;
	}
	else if (bIsTeleporting)
	{
		StartTeleport();
		TeleportingHand = nullptr;
		bSelectingTeleport = false;
	}
}

void AVRPawn::RewindButtonPressedLeft()
{
	LeftHandController->TriggerPressed();
}

void AVRPawn::RewindButtonReleasedLeft()
{
	LeftHandController->TriggerReleased();
}

void AVRPawn::RewindButtonPressedRight()
{
	RightHandController->TriggerPressed();
}

void AVRPawn::RewindButtonReleasedRight()
{
	RightHandController->TriggerReleased();
}

void AVRPawn::RotateButtonPressedLeft()
{
	SetActorRotation(GetActorRotation() - FRotator(0.0, 15.0, 0.0));
}

void AVRPawn::RotateButtonPressedRight()
{
	SetActorRotation(GetActorRotation() + FRotator(0.0, 15.0, 0.0));
}

void AVRPawn::ThumbstickClickPressedLeft()
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to teleport but reference to hand is nullptr"));
	LeftHandController->ThumbstickClickPressed();
}

void AVRPawn::ThumbstickClickPressedRight()
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to teleport but reference to hand is nullptr"));
	RightHandController->ThumbstickClickPressed();
}

#pragma endregion

void AVRPawn::UpdateTeleportMarker()
{
	if (!TeleportingHand) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to teleport but reference to hand is nullptr"));
		return;
	}

	// Do projectile calculation
	FVector ControllerLocation = TeleportingHand->ControllerMesh->GetComponentLocation();

	FPredictProjectilePathParams PredictParams = FPredictProjectilePathParams(1.0f, ControllerLocation, TeleportingHand->ControllerMesh->GetForwardVector() * TeleportRangeVelocity, 2.0f, ECC_GameTraceChannel4);

	FPredictProjectilePathResult Results;
	UGameplayStatics::PredictProjectilePath(this, PredictParams, Results);
	FHitResult Hit = Results.HitResult;

	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Hit.GetComponent()->GetName());
	}
	if (Hit.GetActor() && Hit.GetComponent()->GetName().Contains("Picture"))
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicTime!"));
		PredictParams = FPredictProjectilePathParams(1.0f, ControllerLocation, TeleportingHand->ControllerMesh->GetForwardVector() * TeleportRangeVelocity, 2.0f, ECC_GameTraceChannel3);
		UGameplayStatics::PredictProjectilePath(this, PredictParams, Results);
		Hit = Results.HitResult;
	}

	// Prepare teleport mesh
	for (UStaticMeshComponent* TeleportMesh : TeleportPathComponents)
	{
		TeleportMesh->SetVisibility(false);
	}
	TeleportPath->ClearSplinePoints(false);

	// Update Teleport Path Spline
	for (int32 i = 0; i < Results.PathData.Num(); i++)
	{
		FPredictProjectilePathPointData PointData = Results.PathData[i];
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(PointData.Location);
		FSplinePoint SplinePoint = FSplinePoint(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(SplinePoint, false);
	}

	TeleportPath->UpdateSpline();

	// Put spline points into spline mesh components
	for (int32 i = 0; i < Results.PathData.Num() - 1; i++)
	{
		if (TeleportPathComponents.Num() <= i)
		{
			USplineMeshComponent* NewComponent = NewObject<USplineMeshComponent>(this);
			NewComponent->Mobility = EComponentMobility::Movable;
			NewComponent->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);
			NewComponent->SetStaticMesh(TeleportPathMesh);
			NewComponent->SetMaterial(0, TeleportPathMaterial);
			NewComponent->SetMaterial(1, TeleportPathMaterial);
			NewComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			NewComponent->SetCastShadow(false);
			NewComponent->RegisterComponent();

			TeleportPathComponents.Add(NewComponent);
		}

		TeleportPathComponents[i]->SetVisibility(true);
		FSplineMeshParams SegSplineParams = FSplineMeshParams();
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, SegSplineParams.StartPos, SegSplineParams.StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i + 1, SegSplineParams.EndPos, SegSplineParams.EndTangent);

		TeleportPathComponents[i]->SetStartAndEnd(SegSplineParams.StartPos, SegSplineParams.StartTangent, SegSplineParams.EndPos, SegSplineParams.EndTangent);
	}

	if (Hit.GetActor() && Hit.GetActor()->Tags.Contains(TEXT("Floor")) && FVector::DotProduct(Hit.Normal, FVector::UpVector) > 0.5)
	{
		FVector HitLocation = Hit.Location;

		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(HitLocation);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}
}

void AVRPawn::StartTeleport()
{
	if (DestinationMarker->IsVisible())
	{
		EndTeleport();
	}
	else
	{
		HideTeleportMesh();
	}
}

void AVRPawn::EndTeleport()
{
	HideTeleportMesh();
	DestinationMarker->SetVisibility(false);
	SetActorLocation(DestinationMarker->GetComponentLocation() + PlayerHeight * FVector::UpVector * 100.0f);
}

void AVRPawn::HideTeleportMesh()
{
	for (UStaticMeshComponent* TeleportMesh : TeleportPathComponents)
	{
		TeleportMesh->SetVisibility(false);
	}
}