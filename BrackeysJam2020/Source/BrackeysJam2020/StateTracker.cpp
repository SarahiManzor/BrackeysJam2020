#include "StateTracker.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

UStateTracker::UStateTracker()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsRecording = true;
	bIgnoreDuplicateLocations = true;
}


// Called when the game starts
void UStateTracker::BeginPlay()
{
	Super::BeginPlay();

	AStaticMeshActor* OwnerActor = Cast<AStaticMeshActor>(GetOwner());
	if (!OwnerActor) return;

	OwnerMesh = OwnerActor->GetStaticMeshComponent();
	if (!OwnerMesh) return;
	
	bHasGravity = OwnerMesh->IsGravityEnabled();
	UE_LOG(LogTemp, Warning, TEXT("Gravity"));
}


// Called every frame
void UStateTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Todo account for frame rate
	if (bIsRecording) Record();
	else Rewind();

	/*if (PointsInTime.Num() > 350.0)
		bIsRecording = false;*/
}

void UStateTracker::SetRewinding(bool IsRewinding)
{
	bIsRecording = !IsRewinding;
}

void UStateTracker::Record()
{
	FPointInTime NewTime = FPointInTime();
	NewTime.Location = OwnerMesh->GetComponentLocation();
	NewTime.Rotation = OwnerMesh->GetComponentRotation();
	NewTime.Velocity = OwnerMesh->GetComponentVelocity();

	if (PointsInTime.Num() <= 0 || !PointsInTime[0].Location.Equals(NewTime.Location))
	{
		PointsInTime.Insert(NewTime, 0);
	}
}

void UStateTracker::Rewind()
{
	if (PointsInTime.Num() <= 0)
	{
		return;
	}

	OwnerMesh->SetEnableGravity(false);
	OwnerMesh->SetSimulatePhysics(false);	

	FPointInTime LatestTime = PointsInTime[0];
	OwnerMesh->SetWorldLocation(LatestTime.Location);
	OwnerMesh->SetWorldRotation(LatestTime.Rotation);
	LastVelocity = LatestTime.Velocity;
	PointsInTime.RemoveAt(0);

	// Todo account for frame rate
	if (PointsInTime.Num() > 70.0 * 60.0)
	{
		PointsInTime.RemoveAt(PointsInTime.Num() - 1);
	}
}

void UStateTracker::Play()
{
	bIsRecording = true;
	OwnerMesh->SetEnableGravity(bHasGravity);
	OwnerMesh->SetSimulatePhysics(true);
	OwnerMesh->SetPhysicsLinearVelocity(LastVelocity);
}

bool UStateTracker::IsRewinding()
{
	return !bIsRecording;
}

FVector UStateTracker::GetLocation()
{
	return GetOwner()->GetActorLocation();
}
