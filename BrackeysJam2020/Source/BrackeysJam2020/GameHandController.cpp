// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHandController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "StateTracker.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

AGameHandController::AGameHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlappedStateTracker = nullptr;
	RewindStateTracker = nullptr;
	bTryingGrab = false;
	bSelectingRewind = false;
	HeldComponent = nullptr;
	bInHand = false;
	LastYAxisVal = 0.0f;

	BeamParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BeamParticles"));
	BeamParticles->SetupAttachment(ControllerMesh);

	MagicFrame = nullptr;
}

void AGameHandController::BeginPlay()
{
	Super::BeginPlay();

	ControllerMesh->OnComponentBeginOverlap.AddDynamic(this, &AGameHandController::OnComponentBeginOverlap);
	ControllerMesh->OnComponentEndOverlap.AddDynamic(this, &AGameHandController::OnComponentEndOverlap);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		UStateTracker* Tracker = Cast<UStateTracker>(Actor->GetComponentByClass(UStateTracker::StaticClass()));
		if (Tracker)
		{
			RewindTargets.Add(Tracker);
		}
	}
}

void AGameHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HeldComponent)
	{
		UpdateRewindVisual();
		if (bSelectingRewind)
			ManageRewind();
	}
	else
	{
		BeamParticles->SetFloatParameter(TEXT("ColourAlpha"), 1.0);
		BeamParticles->SetVectorParameter(TEXT("Colour"), FVector(0.0, 0.0, 20.0));

		FVector ControllerLocation = ControllerMesh->GetComponentLocation();
		FVector ControllerForward = ControllerMesh->GetForwardVector();
		FVector TargetLocation = ControllerMesh->GetComponentTransform().TransformPosition(HeldComponentPreviousLocation);
		BeamParticles->SetVectorParameter(TEXT("SourceLocation"), ControllerLocation);
		BeamParticles->SetVectorParameter(TEXT("TargetLocation"), TargetLocation);
		BeamParticles->SetVectorParameter(TEXT("SourceTangent"), ControllerForward * FVector::Dist(TargetLocation, ControllerLocation) * 0.1);
	}
}

bool AGameHandController::CanTeleport()
{
	return !(bSelectingRewind || bTryingGrab);
}

void AGameHandController::Grab()
{
	bTryingGrab = true;
	UE_LOG(LogTemp, Warning, TEXT("Grip Pressed"));
	if (!OverlappedComponent || !OverlappedComponent->IsSimulatingPhysics()) return;

	HeldComponent = OverlappedComponent;

	// Nullify actor physics
	bHeldHasGravity = HeldComponent->IsGravityEnabled();
	HeldLinearDamping = HeldComponent->GetLinearDamping();
	HeldAngularDamping = HeldComponent->GetAngularDamping();
	HeldComponentMass = HeldComponent->GetMass();

	HeldComponent->SetEnableGravity(false);
	HeldComponent->SetLinearDamping(10.0);
	HeldComponent->SetAngularDamping(10.0);
	HeldComponent->SetMassOverrideInKg(NAME_None, 10.0f, true);

	if (HeldComponent->GetOwner()->GetName().Contains("Magic"))
	{
		MagicFrame = HeldComponent->GetOwner();
	}

	UE_LOG(LogTemp, Warning, TEXT("Block held"));
}

void AGameHandController::Release()
{
	bTryingGrab = false;
	UE_LOG(LogTemp, Warning, TEXT("Grip Released"));

	// Restore held actor physics
	if (HeldComponent)
	{
		HeldComponent->SetEnableGravity(bHeldHasGravity);
		HeldComponent->SetLinearDamping(HeldLinearDamping);
		HeldComponent->SetAngularDamping(HeldAngularDamping);
		HeldComponent->SetMassOverrideInKg(NAME_None, HeldComponentMass, true);
		HeldComponent->SetSimulatePhysics(true);
	}
	HeldComponent = nullptr;
	bInHand = false;
}

void AGameHandController::TriggerPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Try Rewind"));
	bSelectingRewind = true;
}

void AGameHandController::TriggerReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("End Rewind"));
	bSelectingRewind = false;
	OverlappedStateTracker = nullptr;
	if (!RewindStateTracker) return;

	RewindStateTracker->RemoveHighlight();
	RewindStateTracker->Play();
	RewindStateTracker = nullptr;
}

void AGameHandController::ThumbStickY(float AxisValue)
{
	LastYAxisVal = AxisValue;
	if (HeldComponent && (!bInHand || AxisValue > 0.1f))
	{
		if (bInHand)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unhanded"));
			HeldActor->SetOwner(nullptr);
			HeldActor->AttachToComponent(nullptr, FAttachmentTransformRules::KeepWorldTransform);
			HeldComponent->SetSimulatePhysics(true);
			if (MagicFrame != nullptr && HeldActor == MagicFrame)
			{
				HeldActorLocation = ControllerMesh->GetComponentTransform().InverseTransformPosition(ControllerMesh->GetComponentLocation() + ControllerMesh->GetForwardVector());
			}
		}
		FVector TargetWorldLocation = ControllerMesh->GetComponentTransform().TransformPosition(HeldActorLocation);

		FVector PreviousLocation = HeldComponent->GetComponentLocation();
		HeldComponentPreviousLocation = ControllerMesh->GetComponentTransform().InverseTransformPosition(PreviousLocation);
		HeldComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		HeldComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		FVector DirPrevToTarget = TargetWorldLocation - PreviousLocation;
		DirPrevToTarget.Normalize();

		float Offset = (TargetWorldLocation - PreviousLocation).Size();

		FVector NewLocation = PreviousLocation;
		NewLocation += DirPrevToTarget * FMath::Sqrt(Offset);

		FVector DirControllerToComponent = ControllerMesh->GetComponentTransform().TransformPosition(HeldActorLocation) - ControllerMesh->GetComponentLocation();
		FVector DirControllerToNewTarget = (NewLocation - ControllerMesh->GetComponentLocation()).GetSafeNormal();

		if (bInHand)
		{
			bInHand = false;
			DirPrevToTarget = DirPrevToTarget = ControllerMesh->GetForwardVector();
		}

		float PrevDistance = (ControllerMesh->GetComponentTransform().TransformPosition(HeldActorLocation) - ControllerMesh->GetComponentLocation()).Size();

		if (!(PrevDistance < 2.0f && AxisValue <= 0.0))
		{
			NewLocation += DirControllerToComponent.GetSafeNormal() * FMath::Sqrt(PrevDistance) * 0.5 * AxisValue;

			float NewDistance = FMath::Min(PrevDistance + FMath::Sqrt(PrevDistance) * 0.5f * AxisValue, 500.0f);
			NewLocation = ControllerMesh->GetComponentLocation() + DirControllerToNewTarget * NewDistance;
			HeldActorLocation = ControllerMesh->GetComponentTransform().InverseTransformPosition(ControllerMesh->GetComponentLocation() + DirControllerToComponent.GetSafeNormal() * NewDistance);
		}

		FVector ToTarget = NewLocation - PreviousLocation;
		if (ToTarget.Size() < 1.0f && FMath::Abs(AxisValue) < 0.1)
		{
			return;
		}

		FVector Impulse = ToTarget * (FMath::Pow(ToTarget.Size(), 2.0) / ToTarget.Size()) * 40.0f;
		float MaxImpulse = 25000.0f;
		if (Impulse.Size() > MaxImpulse)
		{
			Impulse = Impulse.GetSafeNormal() * MaxImpulse;
		}
		HeldComponent->AddImpulse(Impulse * ((1.0f / 70.0f) / GetWorld()->DeltaTimeSeconds));
		//UE_LOG(LogTemp, Warning, TEXT("Impulse: %f"), Impulse.Size());
	}
}

void AGameHandController::ThumbstickClickPressed()
{
	if (!HeldComponent && !RewindStateTracker && MagicFrame)
	{
		MagicFrame->SetActorLocation(ControllerMesh->GetComponentLocation() + ControllerMesh->GetForwardVector() * 10.0f);
	}
}

void AGameHandController::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp == HeldComponent) return;
	UE_LOG(LogTemp, Warning, TEXT("Overlap: %s"), *OverlappedComp->GetName());

	OverlappedActor = OtherActor;

	if (OtherComp == HeldComponent && !bInHand && LastYAxisVal < 0.01f)
	{
		UE_LOG(LogTemp, Warning, TEXT("In hand begin: %s"), *OtherActor->GetName());
		bInHand = true;
		HeldComponent->SetSimulatePhysics(false);
		HeldActor = OtherActor;
		HeldActor->SetOwner(this);
		HeldActor->AttachToComponent(ControllerMesh, FAttachmentTransformRules::KeepWorldTransform);
		if (HeldActor->GetName().Contains("Magic"))
		{
			HeldActor->SetActorLocation(ControllerMesh->GetComponentLocation());
			HeldComponentPreviousLocation = FVector::ZeroVector;
		}
	}
}

void AGameHandController::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedComp == HeldComponent) return;
	if (OtherActor == OverlappedActor) OverlappedActor = nullptr;
	/*UE_LOG(LogTemp, Warning, TEXT("Overlap end: %s"), *OtherActor->GetName());

	if (OverlappedActor == OtherActor)
	{
		OverlappedActor = nullptr;
		OverlappedComponent = nullptr;
	}*/
}

void AGameHandController::UpdateRewindVisual()
{
	// Check for object that can be rewinded
	FVector ControllerLocation = ControllerMesh->GetComponentLocation();
	FVector ControllerForward = ControllerMesh->GetForwardVector();
	FVector TargetLocation = ControllerLocation + ControllerForward * 10000.0;
	if (!RewindStateTracker)
	{
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ControllerLocation, TargetLocation , ECC_GameTraceChannel4);
		if (HitResult.GetActor())
		{
			if (HitResult.GetComponent()->GetName().Contains("Picture"))
			{
				GetWorld()->LineTraceSingleByChannel(HitResult, ControllerLocation, TargetLocation, ECC_GameTraceChannel3);
			}

			BeamParticles->SetFloatParameter(TEXT("ColourAlpha"), 0.01);
			BeamParticles->SetVectorParameter(TEXT("Colour"), FVector(1.0, 1.0, 0.0));

			if (HitResult.GetActor())
			{
				TargetLocation = HitResult.Location;

				UStateTracker* Tracker = Cast<UStateTracker>(HitResult.GetActor()->GetComponentByClass(UStateTracker::StaticClass()));

				if (OverlappedStateTracker != nullptr && Tracker != OverlappedStateTracker)
				{
					// Remove highlight from already highlighted tracker
					OverlappedStateTracker->RemoveHighlight();
				}
				else if (Tracker != nullptr)
				{
					// Add Highlight to new object
					Tracker->AddHighlight();
				}
				OverlappedStateTracker = Tracker; // nullptr if cast failed which is good

				UPrimitiveComponent* OtherComp = Cast<UPrimitiveComponent>(HitResult.GetActor()->GetComponentByClass(UPrimitiveComponent::StaticClass()));
				if (OtherComp->Mobility == EComponentMobility::Movable && !HitResult.GetActor()->Tags.Contains("NoGrab"))
				{
					OverlappedComponent = OtherComp;
					HeldActorLocation = ControllerMesh->GetComponentTransform().InverseTransformPosition(HitResult.GetActor()->GetActorLocation());

					if (bTryingGrab && !HeldComponent)
						Grab();

					if (HeldComponent && OverlappedActor == HitResult.GetActor() && !bInHand)
					{
						bInHand = true;
						HeldComponent->SetSimulatePhysics(false);
						HeldActor = OverlappedActor;
						HeldActor->SetOwner(this);
						HeldActor->AttachToComponent(ControllerMesh, FAttachmentTransformRules::KeepWorldTransform);
						UE_LOG(LogTemp, Warning, TEXT("Debug in hand"));
						if (HeldActor->GetName().Contains("Magic"))
						{
							HeldActor->SetActorLocation(ControllerMesh->GetComponentLocation());
							HeldComponentPreviousLocation = FVector::ZeroVector;
						}
					}
				}
				else
				{
					OverlappedComponent = nullptr;
				}
			}
		}
	}
	else
	{
		// Todo map beam to whatever being rewinded
		TargetLocation = RewindStateTracker->GetLocation();
		BeamParticles->SetFloatParameter(TEXT("ColourAlpha"), 1.0);
		BeamParticles->SetVectorParameter(TEXT("Colour"), FVector(0.0, 20.0, 0.0));
	}

	BeamParticles->SetVectorParameter(TEXT("SourceLocation"), ControllerLocation);
	BeamParticles->SetVectorParameter(TEXT("TargetLocation"), TargetLocation);
	BeamParticles->SetVectorParameter(TEXT("SourceTangent"), ControllerForward * FVector::Dist(TargetLocation, ControllerLocation) * 0.1);
}

void AGameHandController::ManageRewind()
{
	if (!OverlappedStateTracker || OverlappedStateTracker->IsRewinding() || !OverlappedStateTracker->IsSimulatingPhysics()) return;

	if (RewindStateTracker == nullptr)
	{
		RewindStateTracker = OverlappedStateTracker;
		RewindStateTracker->SetRewinding(true);
	}
}
