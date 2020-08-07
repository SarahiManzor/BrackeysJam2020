#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchArea.generated.h"

class UBoxComponent;
class ADoor;
class UStaticMeshComponent;

UCLASS()
class BRACKEYSJAM2020_API ASwitchArea : public AActor
{
	GENERATED_BODY()
	
// ----------Base Functions----------
public:
	ASwitchArea();

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
	bool bHoldOnOverlap;

	UPROPERTY(EditInstanceOnly)
	FVector RelativeHoldLocation;

	// Components
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	// References
	UPROPERTY(EditInstanceOnly)
	ADoor* Door;

	// State
	int32 OverlapCount;

// ----------Custom Functions----------
public:
protected:
private:
	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
