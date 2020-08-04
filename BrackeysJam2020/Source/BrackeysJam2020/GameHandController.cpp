// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHandController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "StateTracker.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AGameHandController::AGameHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlappedStateTracker = nullptr;
	OverlappedActor = nullptr;
	RewindStateTracker = nullptr;
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
	UE_LOG(LogTemp, Warning, TEXT("Grip Pressed"));
	if (!OverlappedActor) return;

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
	if (!RewindStateTracker)
	{
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ControllerLocation, ControllerLocation + ControllerMesh->GetForwardVector() * 10000.0, ECC_Visibility);
		if (HitResult.GetActor())
		{
			DrawDebugLine(GetWorld(), ControllerLocation, HitResult.Location, FColor::Blue, false, 0.05f, (uint8)'\000', 1.0f);
			UStateTracker* Tracker = Cast<UStateTracker>(HitResult.GetActor()->GetComponentByClass(UStateTracker::StaticClass()));
			if (Tracker)
			{
				OverlappedStateTracker = Tracker;
			}
		}
	}
	else
	{
		// Todo map beam to whatever being rewinded
		DrawDebugLine(GetWorld(), ControllerLocation, RewindStateTracker->GetLocation(), FColor::Blue, false, 0.05f, (uint8)'\000', 1.0f);
	}
}

void AGameHandController::ManageRewind()
{
	if (!OverlappedStateTracker || OverlappedStateTracker->IsRewinding()) return;

	if (RewindStateTracker == nullptr)
	{
		RewindStateTracker = OverlappedStateTracker;
		RewindStateTracker->SetRewinding(true);
	}
}
