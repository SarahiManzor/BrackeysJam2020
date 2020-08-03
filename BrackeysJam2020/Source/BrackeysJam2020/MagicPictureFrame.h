// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicPictureFrame.generated.h"

class UStaticMeshComponent;
class USceneCaptureComponent2D;

UCLASS()
class BRACKEYSJAM2020_API AMagicPictureFrame : public AActor
{
	GENERATED_BODY()
	
// ----------Base Functions----------
public:
	AMagicPictureFrame();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

// ----------Variables----------
public:
protected:
private:
	// Config
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* PictureMaterial;

	UPROPERTY(EditDefaultsOnly)
	FName PictureTextureParameterName;

	// Components
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FrameMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PictureMesh;

	UPROPERTY(VisibleAnywhere)
	USceneCaptureComponent2D* SceneCapture;

	// References
	UTextureRenderTarget2D* RenderTarget;

	APlayerController* Player;

	// State

// ----------Custom Functions----------
public:
protected:
private:
	void UpdateCaptureComponent();

};
