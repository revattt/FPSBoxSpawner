// BoxTypes.h
#pragma once

#include "CoreMinimal.h"
#include "BoxTypes.generated.h"

USTRUCT(BlueprintType)
struct FBoxTransform
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    UPROPERTY()
    FVector Scale;
};

USTRUCT(BlueprintType)
struct FBoxTypeData
{
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FLinearColor Color;

    UPROPERTY()
    int32 Health;

    UPROPERTY()
    int32 Score;
};

USTRUCT(BlueprintType)
struct FBoxObjectData
{
    GENERATED_BODY()

    UPROPERTY()
    FString Type;

    UPROPERTY()
    FBoxTransform Transform;
};

USTRUCT(BlueprintType)
struct FBoxDataSet
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBoxTypeData> Types;

    UPROPERTY()
    TArray<FBoxObjectData> Objects;
};