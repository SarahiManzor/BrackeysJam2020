// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UStaticMeshComponent;
class UStateTracker;

UCLASS()
class BRACKEYSJAM2020_API ADoor : public AActor
{
	GENERATED_BODY()
	
// ----------Base Functions----------
public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

// ----------Variables----------
public:
protected:
private:
	// Config

	UPROPERTY(EditInstanceOnly)
		FVector RelativeOpenLocation;

	// Components
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	// References
	UStateTracker* Tracker;

	// State
	FVector CloseLocation;

	bool IsOpening;

// ----------Custom Functions----------
public:
	void Open();
	void Close();
protected:
private:

};
