#pragma once

#include "OceanologyBubbles.h"
#include "OceanologyBubblesSettings.generated.h"

class UMaterialInstance;
class UNiagaraSystem;

UENUM(BlueprintType)
enum EBubbleActivation
{
	Disabled,
	Enabled,
	EnabledOnInteraction
};

USTRUCT(BlueprintType)
struct FOceanologyBubblesSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bubbles Settings")
	TEnumAsByte<EBubbleActivation> EnableBubbles = Disabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bubbles Settings")
	FOceanologyBubbles Bubbles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bubbles Settings")
	int32 BubbleSpawnCount = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bubbles Settings")
	double BubbleSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bubbles Settings")
	TSoftObjectPtr<UMaterialInterface> BubblesMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bubbles Settings")
	TSoftObjectPtr<UNiagaraSystem> BubblesEffect = nullptr;

	FOceanologyBubblesSettings()
	{
	}
};
