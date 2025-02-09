// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BoxSpawnerCharacter.h"
//#include "BoxSpawnerProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "BoxActor.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::OnFire()
{
    if (Character == nullptr || Character->GetController() == nullptr)
    {
        return;
    }

    // Try and fire a ray
    FVector Location;
    FRotator Rotation;
    Character->GetController()->GetPlayerViewPoint(Location, Rotation);

    FVector Start = Location;
    FVector End = Start + (Rotation.Vector() * 10000.f); // 10000 units (100 meters) forward

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);

    if (bHit)
    {
        ABoxActor* BoxActor = Cast<ABoxActor>(Hit.GetActor());
        if (BoxActor)
        {
            // Create a point damage event
            FPointDamageEvent DamageEvent(1.0f, Hit, Rotation.Vector(), nullptr);

            // Apply damage using the proper TakeDamage function
            BoxActor->TakeDamage(1.0f, DamageEvent, Character->GetController(), Character);
        }
    }

    // Try and play the sound if specified
    if (FireSound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
    }

    // Try and play a firing animation if specified
    if (FireAnimation != nullptr)
    {
        // Get the animation object for the arms mesh
        UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
        if (AnimInstance != nullptr)
        {
            AnimInstance->Montage_Play(FireAnimation, 1.f);
        }
    }
}

bool UTP_WeaponComponent::AttachWeapon(ABoxSpawnerCharacter* TargetCharacter)
{
    // Validate Character Reference
    if (!TargetCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("Attempted to attach weapon to null character"));
        return false;
    }

    // Prevent Multiple Weapon Attachments
    if (TargetCharacter->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Character already has a weapon attached"));
        return false;
    }

    Character = TargetCharacter;

    // Consistent Attachment Configuration
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

    // Prioritized Socket Names
    const TArray<FName> PossibleSockets = {
        FName("GripPoint"),     // Most specific
        FName("hand_r"),        // Right hand fallback
        FName("weapon_socket"), // Generic weapon socket
        FName("hand_l")         // Left hand last resort
    };

    // Systematic Socket Attachment
    for (const FName& SocketName : PossibleSockets)
    {
        if (Character->GetMesh1P()->DoesSocketExist(SocketName))
        {
            AttachToComponent(Character->GetMesh1P(), AttachmentRules, SocketName);

            UE_LOG(LogTemp, Log, TEXT("Weapon attached to socket: %s"), *SocketName.ToString());

            Character->AddInstanceComponent(this);
            return true;
        }
    }

    // Fallback Logging if No Socket Found
    UE_LOG(LogTemp, Warning, TEXT("No suitable socket found for weapon attachment"));
    return false;
}


void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}