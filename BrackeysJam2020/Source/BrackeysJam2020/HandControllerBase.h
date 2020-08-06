#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandControllerBase.generated.h"

class UMotionControllerComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;

UCLASS()
class BRACKEYSJAM2020_API AHandControllerBase : public AActor
{
	GENERATED_BODY()

	// ----------Base Functions----------
public:
	AHandControllerBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

// ----------Variables----------
public:
	// Components
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControllerMesh;

protected:
private:
	bool bCanTeleport;

	// ----------Custom Functions----------
public:
	virtual bool CanTeleport();

	void SetTrackingSource(EControllerHand Hand);

	virtual void Grab();
	virtual void Release();


	virtual void ThumbStickY(float AxisValue);

	virtual void TriggerPressed();
	virtual void TriggerReleased();

};
