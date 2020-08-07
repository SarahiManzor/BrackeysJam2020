// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Orb.generated.h"

class UStateTracker;
class UStaticMeshComponent;

UCLASS()
class BRACKEYSJAM2020_API AOrb : public AActor
{
	GENERATED_BODY()
	
		// ----------Base Functions----------
public:
	AOrb();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ----------Variables----------
public:
protected:
private:
	// Config
	UPROPERTY(EditAnywhere)
	FVector InitialVelocity;

	// Components
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;
	
	// References

	// State

// ----------Custom Functions----------
public:
	void SetVelocity();
protected:
private:

};
