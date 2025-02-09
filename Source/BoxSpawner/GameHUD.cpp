#include "GameHUD.h"
#include "BoxSpawnerGameMode.h"
#include "Kismet/GameplayStatics.h"

void UGameHUD::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("GameHUD NativeConstruct Called"));

    // Bind to score updates
    if (ABoxSpawnerGameMode* GameMode = Cast<ABoxSpawnerGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GameMode->OnScoreUpdated.AddDynamic(this, &UGameHUD::UpdateScore);
    }
}