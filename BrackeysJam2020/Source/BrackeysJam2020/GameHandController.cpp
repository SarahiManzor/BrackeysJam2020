// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHandController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "StateTracker.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

AGameHandController::AGameHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlappedStateTracker = nullptr;
	OverlappedActor = nullptr;
	RewindStateTracker = nullptr;
	bTryingGrab = false;
	bSelectingRewind = false;

	BeamParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BeamParticles"));
	BeamParticles->SetupAttachment(ControllerMesh);
}

void AGameHandController::BeginPlay()
{
	Super::BeginPlay();

	ControllerMesh->OnComponentBeginOverlap.AddDynamic(this, &AGameHandController::OnComponentBeginOverlap);
	ControllerMesh->OnComponentEndOverlap.AddDynamic(this, &AGameHandController::OnComponentEndOverlap);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		UStateTracker* Tracker = Cast<UStateTracker>(Actor->GetComponentByClass(UStateTracker::StaticClass()));
		if (Tracker)
		{
			RewindTargets.Add(Tracker);
		}
	}
}

void AGameHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRewindVisual();
	if (bSelectingRewind)
		ManageRewind();
}

void AGameHandController::Grab()
{
	bTryingGrab = true;
	UE_LOG(LogTemp, Warning, TEXT("Grip Pressed"));
	if (!OverlappedActor || !OverlappedComponent->IsSimulatingPhysics()) return;

	HeldActor = OverlappedActor;
	HeldComponent = OverlappedComponent;

	// Nullify actor physics
	HeldComponent->SetSimulatePhysics(false);

	HeldActor->SetOwner(this);
	HeldActor->AttachToComponent(ControllerMesh, FAttachmentTransformRules::KeepWorldTransform);

	UE_LOG(LogTemp, Warning, TEXT("Block held"));
}

void AGameHandController::Release()
{
	bTryingGrab = false;
	UE_LOG(LogTemp, Warning, TEXT("Grip Released"));
	if (!HeldActor) return;

	// Restore held actor physics
	if (HeldActor->GetOwner() == this)
	{
		HeldComponent->SetSimulatePhysics(true);
		HeldActor->SetOwner(nullptr);
		HeldActor->AttachToComponent(nullptr, FAttachmentTransformRules::KeepWorldTransform);
	}

	HeldActor = nullptr;
}

void AGameHandController::TriggerPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Try Rewind"));
	bSelectingRewind = true;
}

void AGameHandController::TriggerReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("End Rewind"));
	bSelectingRewind = false;
	OverlappedStateTracker = nullptr;
	if (!RewindStateTracker) return;

	RewindStateTracker->Play();
	RewindStateTracker = nullptr;
}

void AGameHandController::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap begin: %s"), *OtherActor->GetName());

	if (OtherComp->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn) == ECR_Overlap)
	{
		UE_LOG(LogTemp, Warning, TEXT("With block!"));
		OverlappedComponent = OtherComp;
		OverlappedActor = OtherActor;

		if (bTryingGrab)
			Grab();
	}
}

void AGameHandController::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap end: %s"), *OtherActor->GetName());

	if (OverlappedActor == OtherActor)
	{
		OverlappedActor = nullptr;
		OverlappedComponent = nullptr;
	}
}

void AGameHandController::UpdateRewindVisual()
{
	// Check for object that can be rewinded

	FVector ControllerLocation = ControllerMesh->GetComponentLocation();
	FVector ControllerForward = ControllerMesh->GetForwardVector();
	FVector TargetLocation = FVector::ZeroVector;
	if (!RewindStateTracker)
	{
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ControllerLocation, ControllerLocation + ControllerForward * 10000.0, ECC_Visibility);
		if (HitResult.GetActor())
		{
			TargetLocation = HitResult.Location;
			BeamParticles->SetFloatParameter(TEXT("ColourAlpha"), 0.01);
			BeamParticles->SetVectorParameter(TEXT("Colour"), FVector(1.0, 1.0, 0.0));

			UStateTracker* Tracker = Cast<UStateTracker>(HitResult.GetActor()->GetComponentByClass(UStateTracker::StaticClass()));
			OverlappedStateTracker = Tracker; // nullptr if cast failed which is good
		}
	}
	else
	{
		// Todo map beam to whatever being rewinded
		TargetLocation = RewindStateTracker->GetLocation();
		BeamParticles->SetFloatParameter(TEXT("ColourAlpha"), 1.0);
		BeamParticles->SetVectorParameter(TEXT("Colour"), FVector(0.0, 20.0, 0.0));
	}

	BeamParticles->SetVectorParameter(TEXT("SourceLocation"), ControllerLocation);
	BeamParticles->SetVectorParameter(TEXT("TargetLocation"), TargetLocation);
	BeamParticles->SetVectorParameter(TEXT("SourceTangent"), ControllerForward * FVector::Dist(TargetLocation, ControllerLocation) * 0.1);
}

void AGameHandController::ManageRewind()
{
	if (!OverlappedStateTracker || OverlappedStateTracker->IsRewinding() || !OverlappedStateTracker->IsSimulatingPhysics()) return;

	if (RewindStateTracker == nullptr)
	{
		RewindStateTracker = OverlappedStateTracker;
		RewindStateTracker->SetRewinding(true);
	}
}
