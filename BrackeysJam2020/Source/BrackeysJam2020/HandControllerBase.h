#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandControllerBase.generated.h"

class UMotionControllerComponent;
class UStaticMeshComponent;

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
protected:
	// Components
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControllerMesh;

private:

	// ----------Custom Functions----------
public:
	void SetTrackingSource(EControllerHand Hand);

};