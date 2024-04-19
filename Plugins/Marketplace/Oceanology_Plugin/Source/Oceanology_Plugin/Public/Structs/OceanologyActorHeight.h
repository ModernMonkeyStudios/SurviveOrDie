#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyActorHeight.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyActorHeight
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Actor Height", DisplayName="LocationOffset")
	FLinearColor LocationOffset = FLinearColor(0.0, 0.0, 0.0, 1.0);

	FOceanologyActorHeight()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyActorHeightHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Actor Height")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyActorHeight& ActorHeight)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetVectorParameterValue("LocationOffset", ActorHeight.LocationOffset);
	}

	UFUNCTION(BlueprintPure, Category="ActorHeight")
	static void LerpActorHeight(
		const FOceanologyActorHeight& A, 
		const FOceanologyActorHeight& B, 
		const double Alpha, 
		FOceanologyActorHeight& OutResult
	)
	{
		FOceanologyActorHeight LocalResult;
		OutResult = LocalResult;
	}
};
