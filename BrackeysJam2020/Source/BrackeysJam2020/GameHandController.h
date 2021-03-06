// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HandControllerBase.h"
#include "GameHandController.generated.h"

class UStateTracker;

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
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* BeamParticles;

	// References
	UPrimitiveComponent* HeldComponent;
	FVector HeldComponentPreviousLocation; // Relative to controller
	FVector HeldActorLocation; // Relative to controller
	AActor* HeldActor;
	AActor* OverlappedActor;
	bool bHeldHasGravity;
	float HeldLinearDamping;
	float HeldAngularDamping;
	bool HeldComponentUsingMass;
	float HeldComponentMass;
	bool bInHand;

	float LastYAxisVal;

	UPrimitiveComponent* OverlappedComponent;

	UStateTracker* OverlappedStateTracker;
	UStateTracker* RewindStateTracker;

	// List of possible targets use to add aim assist
	TArray<UStateTracker*> RewindTargets;

	AActor* MagicFrame;

	// State
	bool bSelectingRewind;
	bool bTryingGrab;

// ----------Custom Functions----------
public:
	virtual bool CanTeleport() override;

	virtual void Grab() override;
	virtual void Release() override;

	virtual void TriggerPressed() override;
	virtual void TriggerReleased() override;

	virtual void ThumbStickY(float AxisValue) override;

	virtual void ThumbstickClickPressed() override;
protected:
private:
	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateRewindVisual();
	void ManageRewind();
	
};
