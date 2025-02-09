// BoxSpawnerSubsystem.cpp
#include "BoxSpawnerSubsystem.h"
#include "BoxActor.h"
#include "Json.h"
#include "JsonUtilities.h"

void UBoxSpawnerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UBoxSpawnerSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UBoxSpawnerSubsystem::FetchAndSpawnBoxes()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBoxSpawnerSubsystem::OnHttpResponse);
    HttpRequest->SetURL("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json");
    HttpRequest->SetVerb("GET");
    HttpRequest->ProcessRequest();
}

void UBoxSpawnerSubsystem::OnHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    UE_LOG(LogTemp, Warning, TEXT("HTTP Response Received: %s"),
        bSuccess ? TEXT("Success") : TEXT("Failed"));
    UE_LOG(LogTemp, Warning, TEXT("Response Content: %s"),
        *Response->GetContentAsString());

    if (!bSuccess || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to download box data"));
        return;
    }

    const FString JsonString = Response->GetContentAsString();
    FBoxDataSet BoxData = ParseJSON(JsonString);
    SpawnBoxes(BoxData);
}

FBoxDataSet UBoxSpawnerSubsystem::ParseJSON(const FString& JsonString)
{
    FBoxDataSet BoxData;

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Parse types array
        const TArray<TSharedPtr<FJsonValue>>* TypesArray;
        if (JsonObject->TryGetArrayField(TEXT("types"), TypesArray))
        {
            for (const auto& TypeValue : *TypesArray)
            {
                const TSharedPtr<FJsonObject>& TypeObj = TypeValue->AsObject();
                FBoxTypeData TypeData;

                TypeData.Name = TypeObj->GetStringField(TEXT("name"));

                const TArray<TSharedPtr<FJsonValue>>* ColorArray;
                if (TypeObj->TryGetArrayField(TEXT("color"), ColorArray))
                {
                    TypeData.Color = FLinearColor(
                        (*ColorArray)[0]->AsNumber(),
                        (*ColorArray)[1]->AsNumber(),
                        (*ColorArray)[2]->AsNumber()
                    );
                }

                TypeData.Health = TypeObj->GetIntegerField(TEXT("health"));
                TypeData.Score = TypeObj->GetIntegerField(TEXT("score"));

                BoxData.Types.Add(TypeData);
            }
        }

        // Parse objects array
        const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;
        if (JsonObject->TryGetArrayField(TEXT("objects"), ObjectsArray))
        {
            for (const auto& ObjectValue : *ObjectsArray)
            {
                const TSharedPtr<FJsonObject>& ObjectObj = ObjectValue->AsObject();
                FBoxObjectData ObjectData;

                ObjectData.Type = ObjectObj->GetStringField(TEXT("type"));

                const TSharedPtr<FJsonObject>& TransformObj = ObjectObj->GetObjectField(TEXT("transform"));

                // Parse location
                const TArray<TSharedPtr<FJsonValue>>* LocationArray;
                if (TransformObj->TryGetArrayField(TEXT("location"), LocationArray))
                {
                    ObjectData.Transform.Location = FVector(
                        (*LocationArray)[0]->AsNumber(),
                        (*LocationArray)[1]->AsNumber(),
                        (*LocationArray)[2]->AsNumber()
                    );
                }

                // Parse rotation
                const TArray<TSharedPtr<FJsonValue>>* RotationArray;
                if (TransformObj->TryGetArrayField(TEXT("rotation"), RotationArray))
                {
                    ObjectData.Transform.Rotation = FRotator(
                        (*RotationArray)[1]->AsNumber(), // Pitch
                        (*RotationArray)[2]->AsNumber(), // Yaw
                        (*RotationArray)[0]->AsNumber()  // Roll
                    );
                }

                // Parse scale
                const TArray<TSharedPtr<FJsonValue>>* ScaleArray;
                if (TransformObj->TryGetArrayField(TEXT("scale"), ScaleArray))
                {
                    ObjectData.Transform.Scale = FVector(
                        (*ScaleArray)[0]->AsNumber(),
                        (*ScaleArray)[1]->AsNumber(),
                        (*ScaleArray)[2]->AsNumber()
                    );
                }

                BoxData.Objects.Add(ObjectData);
            }
        }
    }

    return BoxData;
}

void UBoxSpawnerSubsystem::SpawnBoxes(const FBoxDataSet& BoxData)
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (const FBoxObjectData& ObjectData : BoxData.Objects)
    {
        // Find matching type data
        const FBoxTypeData* TypeData = BoxData.Types.FindByPredicate([&](const FBoxTypeData& Type) {
            return Type.Name == ObjectData.Type;
            });

        if (!TypeData) continue;

        // Spawn the box
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ABoxActor* BoxActor = World->SpawnActor<ABoxActor>(
            ABoxActor::StaticClass(),
            ObjectData.Transform.Location,
            ObjectData.Transform.Rotation,
            SpawnParams
        );

        if (BoxActor)
        {
            BoxActor->SetActorScale3D(ObjectData.Transform.Scale);
            BoxActor->Initialize(*TypeData);
        }
    }
}