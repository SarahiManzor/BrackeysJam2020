// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHandController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

AGameHandController::AGameHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlappedActor = nullptr;
}

void AGameHandController::BeginPlay()
{
	Super::BeginPlay();

	ControllerMesh->OnComponentBeginOverlap.AddDynamic(this, &AGameHandController::OnComponentBeginOverlap);
	ControllerMesh->OnComponentEndOverlap.AddDynamic(this, &AGameHandController::OnComponentEndOverlap);
}

void AGameHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
