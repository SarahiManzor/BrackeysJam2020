#include "SwitchArea.h"
#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ASwitchArea::ASwitchArea()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->SetupAttachment(Root);

	OverlapCount = 0;
	bHoldOnOverlap = false;
	RelativeHoldLocation = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ASwitchArea::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASwitchArea::OnComponentBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ASwitchArea::OnComponentEndOverlap);

	RelativeHoldLocation = GetTransform().TransformPosition(RelativeHoldLocation);
}

// Called every frame
void ASwitchArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASwitchArea::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap"));
	OverlapCount++;
	if (Door != nullptr)
	{
		Door->Open();
		OtherComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		OtherComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		OtherActor->SetActorLocation(RelativeHoldLocation);
	}
}

void ASwitchArea::OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
	OverlapCount --;
	if (OverlapCount <= 0 && Door != nullptr)
	{
		Door->Close();
	}
}

