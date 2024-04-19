#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyBubbles.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyBubbles
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bubbles", DisplayName="BubbleColor")
	FLinearColor BubbleColor = FLinearColor(0.15, 0.5, 1.0, 1.0);

	FOceanologyBubbles()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyBubblesHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Bubbles")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyBubbles& Bubbles)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetVectorParameterValue("BubbleColor", Bubbles.BubbleColor);
	}

	UFUNCTION(BlueprintPure, Category="Bubbles")
	static void LerpBubbles(
		const FOceanologyBubbles& A, 
		const FOceanologyBubbles& B, 
		const double Alpha, 
		FOceanologyBubbles& OutResult
	)
	{
		FOceanologyBubbles LocalResult;
		OutResult = LocalResult;
	}
};
