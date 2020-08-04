// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateTracker.generated.h"

class UStaticMeshComponent;

USTRUCT()
struct BRACKEYSJAM2020_API FPointInTime
{
	GENERATED_USTRUCT_BODY()
	FVector Location;
	FRotator Rotation;
	FVector Velocity;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRACKEYSJAM2020_API UStateTracker : public UActorComponent
{
	GENERATED_BODY()

		// ----------Base Functions----------
public:
	UStateTracker();

protected:
	virtual void BeginPlay() override;

public:
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	// ----------Variables----------
public:
protected:
private:
	// Config
	bool bHasGravity;
	bool bIsRecording;
	bool bIgnoreDuplicateLocations;

	// Components

	// References
	UStaticMeshComponent* OwnerMesh;
	UStaticMeshComponent* HighlightMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* HighlightMaterial;

	// State
	TArray<FPointInTime> PointsInTime;
	FVector LastVelocity;

// ----------Custom Functions----------
public:
	void SetRewinding(bool IsRewinding);
	void Play();
	bool IsRewinding();
	bool IsSimulatingPhysics();
	FVector GetLocation();

	void AddHighlight();
	void RemoveHighlight();
protected:
private:
	void Record();
	void Rewind();

		
};
