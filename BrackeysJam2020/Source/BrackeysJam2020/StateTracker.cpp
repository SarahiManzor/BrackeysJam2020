#include "StateTracker.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Orb.h"

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

	AActor* OwnerActor = Cast<AActor>(GetOwner());
	if (!OwnerActor) return;

	OwnerMesh = Cast<UStaticMeshComponent>(OwnerActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!OwnerMesh) return;
	
	bHasGravity = OwnerMesh->IsGravityEnabled();
	
	HighlightMesh = NewObject<UStaticMeshComponent>(this, TEXT("HighlightMesh"));
	HighlightMesh->SetSimulatePhysics(false);
	HighlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HighlightMesh->RegisterComponent();
	HighlightMesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::KeepWorldTransform);
	HighlightMesh->SetStaticMesh(OwnerMesh->GetStaticMesh());
	HighlightMesh->SetWorldLocation(OwnerMesh->GetComponentLocation());
	HighlightMesh->SetWorldRotation(OwnerMesh->GetComponentRotation());
	HighlightMesh->SetRelativeScale3D(FVector(1.1, 1.1, 1.1));
	HighlightMesh->SetMaterial(0, HighlightMaterial);
	HighlightMesh->SetVisibility(false);
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
	NewTime.LinearVelocity = OwnerMesh->GetPhysicsLinearVelocity();
	NewTime.AngularVelocity = OwnerMesh->GetPhysicsAngularVelocityInDegrees();
	UE_LOG(LogTemp, Warning, TEXT("Recorded velociity: %f"), *LastLinearVelocity.ToCompactString());

	if (PointsInTime.Num() <= 0 || !PointsInTime[0].Location.Equals(NewTime.Location))
	{
		PointsInTime.Insert(NewTime, 0);
	}
}

void UStateTracker::Rewind()
{
	if (PointsInTime.Num() <= 5)
	{
		return;
	}

	OwnerMesh->SetEnableGravity(false);
	OwnerMesh->SetSimulatePhysics(false);
	OwnerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FPointInTime LatestTime = PointsInTime[0];
	OwnerMesh->SetWorldLocation(LatestTime.Location);
	OwnerMesh->SetWorldRotation(LatestTime.Rotation);
	LastLinearVelocity = LatestTime.LinearVelocity;
	LastAngularVelocity = LatestTime.AngularVelocity;
	PointsInTime.RemoveAt(0);

	// Todo account for frame rate
	if (PointsInTime.Num() > 70.0 * 120.0)
	{
		PointsInTime.RemoveAt(PointsInTime.Num() - 1);
	}
}

void UStateTracker::Play()
{
	bIsRecording = true;
	OwnerMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OwnerMesh->SetEnableGravity(bHasGravity);
	OwnerMesh->SetSimulatePhysics(true);
	OwnerMesh->SetPhysicsLinearVelocity(LastLinearVelocity);
	OwnerMesh->SetPhysicsAngularVelocityInDegrees(LastAngularVelocity);

	AOrb* Orb = Cast<AOrb>(OwnerMesh->GetOwner());
	if (Orb && PointsInTime.Num() <= 5)
	{
		Orb->SetVelocity();
	}
}

bool UStateTracker::IsRewinding()
{
	return !bIsRecording;
}

bool UStateTracker::IsSimulatingPhysics()
{
	return OwnerMesh->IsSimulatingPhysics();
}

FVector UStateTracker::GetLocation()
{
	return GetOwner()->GetActorLocation();
}

void UStateTracker::AddHighlight()
{
	HighlightMesh->SetVisibility(true);
}

void UStateTracker::RemoveHighlight()
{
	HighlightMesh->SetVisibility(false);
}
