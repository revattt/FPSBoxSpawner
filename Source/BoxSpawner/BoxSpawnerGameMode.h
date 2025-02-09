#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoxSpawnerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, NewScore);

UCLASS()
class BOXSPAWNER_API ABoxSpawnerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Scoring")
    void AddScore(int32 Points);

    UFUNCTION(BlueprintPure, Category = "Scoring")
    int32 GetCurrentScore() const { return CurrentScore; }

    UPROPERTY(BlueprintAssignable, Category = "Scoring")
    FOnScoreUpdated OnScoreUpdated;

    // Register BoxActor for destruction tracking
    void RegisterBox(AActor* BoxActor);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(BlueprintReadOnly, Category = "Scoring", meta = (AllowPrivateAccess = "true"))
    int32 CurrentScore = 0;

    // Callback for when a box gets destroyed
    UFUNCTION()
    void OnBoxDestroyed(AActor* DestroyedActor);
};
