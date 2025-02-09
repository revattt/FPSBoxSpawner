#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BoxTypes.h"
#include "Http.h"
#include "BoxSpawnerSubsystem.generated.h"

UCLASS()
class BOXSPAWNER_API UBoxSpawnerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void FetchAndSpawnBoxes();

private:
    void OnHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void SpawnBoxes(const FBoxDataSet& BoxData);

    FBoxDataSet ParseJSON(const FString& JsonString);
};