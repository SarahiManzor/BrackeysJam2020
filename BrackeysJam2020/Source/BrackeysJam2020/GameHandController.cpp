// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHandController.h"
#include "Components/StaticMeshComponent.h"

AGameHandController::AGameHandController()
{

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

void AGameHandController::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap begin: %s"), *OverlappedComp->GetName());
}

void AGameHandController::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap end: %s"), *OverlappedComp->GetName());
}
