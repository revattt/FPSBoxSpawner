#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameHUD.generated.h"

UCLASS()
class BOXSPAWNER_API UGameHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Score")
    void UpdateScore(int32 NewScore);

protected:
    virtual void NativeConstruct() override;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ScoreText;
};