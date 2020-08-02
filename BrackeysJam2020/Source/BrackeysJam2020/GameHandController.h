// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HandControllerBase.h"
#include "GameHandController.generated.h"

/**
 * 
 */
UCLASS()
class BRACKEYSJAM2020_API AGameHandController : public AHandControllerBase
{
	GENERATED_BODY()

// ----------Base Functions----------
public:
	AGameHandController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

// ----------Variables----------
public:
protected:
private:
	// Config

	// Components

	// References
	AActor* HeldActor;
	AActor* OverlappedActor;

	// State

// ----------Custom Functions----------
public:
protected:
private:
	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
