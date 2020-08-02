// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

class AHandControllerBase;
class UCameraComponent;

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

	// Components
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* Camera;

	// References
	UPROPERTY()
		AHandControllerBase* RightHandController;

	UPROPERTY()
		AHandControllerBase* LeftHandController;

	// State

// ----------Custom Functions----------
public:
protected:
private:
	void GrabButtonPressedLeft();
	void GrabButtonReleasedLeft();

	void GrabButtonPressedRight();
	void GrabButtonReleasedRight();

};
