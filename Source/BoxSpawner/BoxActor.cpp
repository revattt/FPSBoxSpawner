// BoxActor.cpp
#include "BoxActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

ABoxActor::ABoxActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and setup the box mesh
    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
    RootComponent = BoxMesh;

    // Load the cube static mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
    if (MeshAsset.Succeeded())
    {
        BoxMesh->SetStaticMesh(MeshAsset.Object);
    }

    // Load the base material
    static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Materials/M_Box"));
    if (MaterialAsset.Succeeded())
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialAsset.Object, this);
        BoxMesh->SetMaterial(0, DynamicMaterial);
    }

    // Setup collision and physics
    BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxMesh->SetCollisionObjectType(ECollisionChannel::ECC_Destructible);
    BoxMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

void ABoxActor::BeginPlay()
{
    Super::BeginPlay();
}

void ABoxActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ABoxActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth--;

    if (CurrentHealth <= 0)
    {
        // Optional: Add visual effect before destruction
        // Optional: Add score to game mode here
        Destroy();
    }

    return ActualDamage;
}

void ABoxActor::Initialize(const FBoxTypeData& BoxData)
{
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), BoxData.Color);
    }
    CurrentHealth = BoxData.Health;
    ScoreValue = BoxData.Score;
}