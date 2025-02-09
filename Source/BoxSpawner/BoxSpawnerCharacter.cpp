#include "BoxSpawnerCharacter.h"
#include "TP_WeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ABoxSpawnerCharacter::ABoxSpawnerCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Create a CameraComponent    
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create the first-person arms mesh
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetupAttachment(FirstPersonCameraComponent); // Arms are attached to the camera
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // Create and attach WeaponComponent to Mesh1P (arms)
    WeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("WeaponComponent"));
    if (WeaponComponent)
    {
        WeaponComponent->SetupAttachment(Mesh1P, TEXT("hand_rSocket")); // Weapon is attached to the arms
    }
}

void ABoxSpawnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (WeaponComponent && Mesh1P)
    {
        if (Mesh1P->DoesSocketExist(TEXT("hand_rSocket")))
        {
            WeaponComponent->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_rSocket"));
            UE_LOG(LogTemplateCharacter, Log, TEXT("Weapon attached to hand_rSocket successfully!"));
        }
        else
        {
            UE_LOG(LogTemplateCharacter, Error, TEXT("Socket 'hand_rSocket' does not exist on Mesh1P!"));
        }
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("WeaponComponent or Mesh1P is NULL in BeginPlay!"));
    }
}

void ABoxSpawnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABoxSpawnerCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABoxSpawnerCharacter::Look);

        // Firing Weapon
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABoxSpawnerCharacter::OnFire);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
    }
}

void ABoxSpawnerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void ABoxSpawnerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void ABoxSpawnerCharacter::OnFire()
{
    if (WeaponComponent)
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("Character is calling OnFire()!"));
        WeaponComponent->OnFire();
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("WeaponComponent is NULL! Make sure it's initialized in Blueprint or Constructor."));
    }
}