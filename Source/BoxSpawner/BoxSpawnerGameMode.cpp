#include "BoxSpawnerGameMode.h"
#include "BoxSpawnerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"  // Required for GEngine->AddOnScreenDebugMessage()

void ABoxSpawnerGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay Called"));
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GameMode: BeginPlay Called"));

    // Initialize score
    CurrentScore = 0;
    UE_LOG(LogTemp, Warning, TEXT("Score Initialized: %d"), CurrentScore);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Score Initialized"));

    // Fetch and spawn boxes from JSON
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
    {
        if (UBoxSpawnerSubsystem* BoxSpawner = GameInstance->GetSubsystem<UBoxSpawnerSubsystem>())
        {
            UE_LOG(LogTemp, Warning, TEXT("BoxSpawner Subsystem Found, Fetching Boxes..."));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Fetching Boxes..."));

            BoxSpawner->FetchAndSpawnBoxes();
            UE_LOG(LogTemp, Warning, TEXT("Box Fetching Started"));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Box Fetching Started"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BoxSpawner Subsystem NOT Found!"));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: BoxSpawner Subsystem NOT Found!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance NOT Found!"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: GameInstance NOT Found!"));
    }
}

// Register BoxActor to track when it is destroyed
void ABoxSpawnerGameMode::RegisterBox(AActor* BoxActor)
{
    if (BoxActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Registering Box: %s"), *BoxActor->GetName());
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Registering Box: %s"), *BoxActor->GetName()));

        BoxActor->OnDestroyed.AddDynamic(this, &ABoxSpawnerGameMode::OnBoxDestroyed);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Attempted to Register a Null BoxActor!"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: Null BoxActor Registration!"));
    }
}

// Called when a BoxActor is destroyed
void ABoxSpawnerGameMode::OnBoxDestroyed(AActor* DestroyedActor)
{
    if (DestroyedActor)
    {
        FString BoxName = DestroyedActor->GetName();
        UE_LOG(LogTemp, Warning, TEXT("Box Destroyed: %s"), *BoxName);

        // Show debug message on-screen
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Box Destroyed: %s"), *BoxName));

        AddScore(10);  // Award 10 points when a box is destroyed
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnBoxDestroyed Called with NULL Actor!"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR: Null DestroyedActor!"));
    }
}


void ABoxSpawnerGameMode::AddScore(int32 Points)
{
    CurrentScore += Points;
    UE_LOG(LogTemp, Log, TEXT("Score Updated: %d"), CurrentScore);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Score Updated: %d"), CurrentScore));

    // Broadcast score update event
    OnScoreUpdated.Broadcast(CurrentScore);
}
