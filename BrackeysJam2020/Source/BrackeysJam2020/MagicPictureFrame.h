// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicPictureFrame.generated.h"

class UStaticMeshComponent;
class USceneCaptureComponent2D;
class UMaterialInstanceDynamic;

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
	UMaterialInterface* PictureMaterialLeft;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* PictureMaterialRight;

	UPROPERTY(EditDefaultsOnly)
	FName PictureTextureParameterName;

	UPROPERTY(EditAnywhere)
	float EyeDistanceFromCenter;

	// Components
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FrameMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PictureMeshLeft;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PictureMeshRight;

	UPROPERTY(VisibleAnywhere)
	USceneCaptureComponent2D* SceneCaptureLeft;
	
	UPROPERTY(VisibleAnywhere)
	USceneCaptureComponent2D* SceneCaptureRight;

	// References
	UTextureRenderTarget2D* RenderTargetLeft;
	UTextureRenderTarget2D* RenderTargetRight;

	UMaterialInstanceDynamic* MIDLeft;
	UMaterialInstanceDynamic* MIDRight;

	APlayerController* Player;

	// State

// ----------Custom Functions----------
public:
protected:
private:
	void UpdateCaptureComponent();
	void UpdatePortalVPMParameters(USceneCaptureComponent2D* CaptureComponent, UMaterialInstanceDynamic* MaterialInstance, const FTransform& CameraTransform);
	float GetFOVForCaptureComponents(const APlayerController* ForPlayerController = nullptr);
	static void UpdateCaptureComponentClipplane(USceneCaptureComponent2D* CaptureComponent, const USceneComponent* TargetPortal, float ClipOffset = -5.0f);
	static void UpdateCaptureComponentClipPlaneTransform(USceneCaptureComponent2D* CaptureComponent, const FTransform& TargetPortal, float ClipOffset = -5.0f);
};
