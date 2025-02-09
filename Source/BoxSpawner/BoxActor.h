// BoxActor.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxTypes.h"
#include "BoxActor.generated.h"

UCLASS()
class BOXSPAWNER_API ABoxActor : public AActor
{
    GENERATED_BODY()

public:
    ABoxActor();
    virtual void Tick(float DeltaTime) override;

    // Only keep the engine's TakeDamage function
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    void Initialize(const FBoxTypeData& BoxData);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BoxMesh;

    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    UPROPERTY()
    int32 CurrentHealth;

    UPROPERTY()
    int32 ScoreValue;
};