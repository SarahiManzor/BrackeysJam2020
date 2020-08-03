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

AMagicPictureFrame::AMagicPictureFrame()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	FrameMesh->SetupAttachment(Root);
	
	PictureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PictureMesh"));
	PictureMesh->SetupAttachment(Root);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(Root);
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
		RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, ViewportX, ViewportY);
		SceneCapture->TextureTarget = RenderTarget;
		SceneCapture->HiddenActors.Add(this);

		if (PictureMaterial)
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(PictureMaterial, this);
			DynamicMaterial->SetTextureParameterValue(PictureTextureParameterName, RenderTarget);

			PictureMesh->SetMaterial(0, DynamicMaterial);
		}
	}
}

void AMagicPictureFrame::UpdateCaptureComponent()
{
	SceneCapture->SetWorldLocation(Player->PlayerCameraManager->GetCameraLocation());
	SceneCapture->SetWorldRotation(Player->PlayerCameraManager->GetCameraRotation());
}

void AMagicPictureFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCaptureComponent();
}

