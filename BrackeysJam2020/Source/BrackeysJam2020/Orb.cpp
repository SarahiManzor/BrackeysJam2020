#include "Orb.h"
#include "Components/StaticMeshComponent.h"
#include "StateTracker.h"

AOrb::AOrb()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	SetRootComponent(MeshComponent);
}

void AOrb::BeginPlay()
{
	Super::BeginPlay();
	SetVelocity();
}

void AOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrb::SetVelocity()
{
	MeshComponent->SetPhysicsLinearVelocity(InitialVelocity);
}

