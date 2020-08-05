#include "MagicPictureFrame.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/GameViewportClient.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "Engine/LocalPlayer.h"

AMagicPictureFrame::AMagicPictureFrame()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	SetRootComponent(FrameMesh);

	PictureMeshLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PictureMeshLeft"));
	PictureMeshLeft->SetupAttachment(FrameMesh);

	PictureMeshRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PictureMeshRight"));
	PictureMeshRight->SetupAttachment(FrameMesh);

	SceneCaptureLeft = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureLeft"));
	SceneCaptureLeft->SetupAttachment(FrameMesh);

	SceneCaptureRight = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureRight"));
	SceneCaptureRight->SetupAttachment(FrameMesh);

	EyeDistanceFromCenter = 2.5f;
}

void AMagicPictureFrame::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerController(this, 0);

	if (Player)
	{
		int32 ViewportX;
		int32 ViewportY;
		Player->GetViewportSize(ViewportX, ViewportY);

		RenderTargetLeft = UKismetRenderingLibrary::CreateRenderTarget2D(this, ViewportX / 2.0 * 1.2, ViewportY);
		SceneCaptureLeft->TextureTarget = RenderTargetLeft;
		SceneCaptureLeft->HiddenActors.Add(this);

		RenderTargetRight = UKismetRenderingLibrary::CreateRenderTarget2D(this, ViewportX / 2.0 * 1.2, ViewportY);
		SceneCaptureRight->TextureTarget = RenderTargetRight;
		SceneCaptureRight->HiddenActors.Add(this);

		float FOV = GetFOVForCaptureComponents(Player);
		SceneCaptureLeft->FOVAngle = FOV;
		SceneCaptureRight->FOVAngle = FOV;

		if (PictureMaterialLeft && PictureMaterialRight)
		{
			UMaterialInstanceDynamic* DynamicMaterialLeft = UMaterialInstanceDynamic::Create(PictureMaterialLeft, this);
			DynamicMaterialLeft->SetTextureParameterValue(PictureTextureParameterName, RenderTargetLeft);

			PictureMeshLeft->SetMaterial(0, DynamicMaterialLeft);

			UMaterialInstanceDynamic* DynamicMaterialRight = UMaterialInstanceDynamic::Create(PictureMaterialRight, this);
			DynamicMaterialRight->SetTextureParameterValue(PictureTextureParameterName, RenderTargetRight);

			PictureMeshRight->SetMaterial(0, DynamicMaterialRight);

			MIDLeft = DynamicMaterialLeft;
			MIDRight = DynamicMaterialRight;
		}
	}
}

void AMagicPictureFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCaptureComponent();
}

#pragma region Credit for the logic here goes to https://github.com/FreetimeStudio/PortalPlugin

void AMagicPictureFrame::UpdateCaptureComponent()
{
	FVector CamLocation = Player->PlayerCameraManager->GetCameraLocation();
	FRotator CamRotation = Player->PlayerCameraManager->GetCameraRotation();
	FVector CamRightVector = Player->PlayerCameraManager->GetActorRightVector();

	FVector CamLocationLeft = CamLocation - CamRightVector * EyeDistanceFromCenter;
	FVector CamLocationRight = CamLocation + CamRightVector * EyeDistanceFromCenter;

	SceneCaptureLeft->SetWorldLocation(CamLocationLeft);
	SceneCaptureLeft->SetWorldRotation(CamRotation);

	SceneCaptureRight->SetWorldLocation(CamLocationRight);
	SceneCaptureRight->SetWorldRotation(CamRotation);

	SceneCaptureLeft->ClipPlaneBase = SceneCaptureRight->ClipPlaneBase = GetActorLocation();

	FVector ClipNormal = GetActorForwardVector() * 0.1;
	if (FVector::DotProduct(GetActorLocation() - CamLocationLeft, ClipNormal) < 0.0)
		ClipNormal *= -1.0;

	SceneCaptureLeft->ClipPlaneNormal = SceneCaptureRight->ClipPlaneNormal = ClipNormal;

	FTransform CameraTransformLeft = FTransform(CamRotation, CamLocationLeft);
	FTransform CameraTransformRight = FTransform(CamRotation, CamLocationRight);
	UpdatePortalVPMParameters(SceneCaptureLeft, MIDLeft, CameraTransformLeft);
	UpdatePortalVPMParameters(SceneCaptureRight, MIDRight, CameraTransformRight);

	UE_LOG(LogTemp, Warning, TEXT("Location %s"), *CamLocation.ToCompactString());
	UE_LOG(LogTemp, Warning, TEXT("Rotation %s"), *CamRotation.ToCompactString());
}

//Credit goes to AgentMilkshake1 https://answers.unrealengine.com/questions/234597/screenspace-portals-on-vr.html
void AMagicPictureFrame::UpdatePortalVPMParameters(USceneCaptureComponent2D* CaptureComponent, UMaterialInstanceDynamic* MaterialInstance, const FTransform& CameraTransform)
{
	if (!IsValid(CaptureComponent) || !IsValid(MaterialInstance))
	{
		return;
	}

	if (!IsValid(CaptureComponent->TextureTarget))
	{
		return;
	}

	float CaptureSizeX = CaptureComponent->TextureTarget->GetSurfaceWidth();
	float CaptureSizeY = CaptureComponent->TextureTarget->GetSurfaceHeight();
	const FTransform& Transform = CameraTransform;
	FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();
	FVector ViewLocation = Transform.GetTranslation();
	// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
	ViewMatrix = ViewMatrix * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));
	CaptureComponent->FOVAngle = GetFOVForCaptureComponents();
	const float FOV = CaptureComponent->FOVAngle * (float)PI / 360.0f;

	// Build projection matrix
	float XAxisMultiplier;
	float YAxisMultiplier;

	if (CaptureSizeX > CaptureSizeY)
	{
		// if the viewport is wider than it is tall
		XAxisMultiplier = 1.0f;
		YAxisMultiplier = CaptureSizeX / CaptureSizeY;
	}
	else
	{
		// if the viewport is taller than it is wide
		XAxisMultiplier = CaptureSizeY / CaptureSizeX;
		YAxisMultiplier = 1.0f;
	}

	FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
		FOV,
		FOV,
		XAxisMultiplier,
		YAxisMultiplier,
		10,
		1000
	);

	const FMatrix ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;

	FVector Xaxis = ViewProjectionMatrix.GetColumn(0);
	FVector Yaxis = ViewProjectionMatrix.GetColumn(1);
	FVector Zaxis = ViewProjectionMatrix.GetColumn(2);
	FVector Waxis = ViewProjectionMatrix.GetColumn(3);

	float XaxisW = ViewProjectionMatrix.M[3][0];
	float YaxisW = ViewProjectionMatrix.M[3][1];
	float ZaxisW = ViewProjectionMatrix.M[3][2];
	float WaxisW = ViewProjectionMatrix.M[3][3];

	MaterialInstance->SetVectorParameterValue("PortalVPM_Xaxis", FLinearColor(Xaxis.X, Xaxis.Y, Xaxis.Z, XaxisW));
	MaterialInstance->SetVectorParameterValue("PortalVPM_Yaxis", FLinearColor(Yaxis.X, Yaxis.Y, Yaxis.Z, YaxisW));
	MaterialInstance->SetVectorParameterValue("PortalVPM_Zaxis", FLinearColor(Zaxis.X, Zaxis.Y, Zaxis.Z, ZaxisW));
	MaterialInstance->SetVectorParameterValue("PortalVPM_Waxis", FLinearColor(Waxis.X, Waxis.Y, Waxis.Z, WaxisW));
}

//Credit goes to AgentMilkshake1 https://answers.unrealengine.com/questions/234597/screenspace-portals-on-vr.html
float AMagicPictureFrame::GetFOVForCaptureComponents(const APlayerController* ForPlayerController)
{
	float ResultFOV = 90.0f;

	if (ForPlayerController != nullptr)
	{
		if (ForPlayerController->PlayerCameraManager != nullptr)
		{
			ResultFOV = ForPlayerController->PlayerCameraManager->GetFOVAngle();
		}
	}

	if (!GEngine)
	{
		return ResultFOV;
	}

	// FOV changes when we have a VR Headset enabled
	if (GEngine->XRSystem.IsValid() && GEngine->IsStereoscopic3D())
	{
		const float EdgeScaling = 1.1f;
		float HFOV, VFOV;
		GEngine->XRSystem->GetHMDDevice()->GetFieldOfView(HFOV, VFOV);
		if (VFOV > 0 && HFOV > 0)
		{
			ResultFOV = FMath::Max(HFOV, VFOV) * EdgeScaling;
			// AspectRatio won't be used until bConstrainAspectRatio is set to true,
			// but it doesn't really matter since HMD calcs its own projection matrix.
			//OutViewInfo.AspectRatio = HFOV / VFOV;
			//OutViewInfo.bConstrainAspectRatio = true;
		}
		else
		{
			FSceneViewProjectionData ProjectionData;

			//SteamVR may not have FOV information, try to it via the current viewport
			ULocalPlayer* LocalPlayer = GEngine->GetGamePlayer(GEngine->GameViewport, 0);
			LocalPlayer->GetProjectionData(GEngine->GameViewport->Viewport, eSSP_FULL, ProjectionData);

			float t = ProjectionData.ProjectionMatrix.M[1][1];
			const float Rad2Deg = 180 / PI;
			ResultFOV = FMath::Atan(1.f / t) * 4.f * Rad2Deg * EdgeScaling;
		}
	}

	return ResultFOV;
}

#pragma endregion