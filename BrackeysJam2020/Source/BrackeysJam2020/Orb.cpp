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

	MeshComponent->AddImpulse(InitialVelocity);	
}

void AOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

