// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

class AHandControllerBase;
class UCameraComponent;
class USplineMeshComponent;
class USplineComponent;

UCLASS()
class BRACKEYSJAM2020_API AVRPawn : public APawn
{
	GENERATED_BODY()

		// ----------Base Functions----------
public:
	AVRPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ----------Variables----------
public:
protected:
private:
	// Config
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> RightHandControllerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> LeftHandControllerClass;

	UPROPERTY(EditAnywhere)
	float TeleportRangeVelocity;

	UPROPERTY(EditAnywhere)
	float PlayerHeight;

	float TeleportThreshold;

	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DestinationMarker;

	UPROPERTY()
	TArray<USplineMeshComponent*> TeleportPathComponents;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* TeleportPathMaterial;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* TeleportPathMesh;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* TeleportPath;

	// References
	UPROPERTY()
	AHandControllerBase* RightHandController;

	UPROPERTY()
	AHandControllerBase* LeftHandController;

	AHandControllerBase* TeleportingHand; // Hand currently teleporting

	// State
	bool bSelectingTeleport;

	// ----------Custom Functions----------
public:
protected:
private:
	void GrabButtonPressedLeft();
	void GrabButtonReleasedLeft();

	void GrabButtonPressedRight();
	void GrabButtonReleasedRight();

	void ThumbStickLeftY(float AxisValue);
	void ThumbStickRightY(float AxisValue);
	
	void RewindButtonPressedLeft();
	void RewindButtonReleasedLeft();

	void RewindButtonPressedRight();
	void RewindButtonReleasedRight();

	void RotateButtonPressedLeft();
	void RotateButtonPressedRight();

	void UpdateTeleportMarker();

	void ManageTeleport(AHandControllerBase* HandController, float AxisValue);
	void StartTeleport();
	void EndTeleport();

	void HideTeleportMesh();
};
