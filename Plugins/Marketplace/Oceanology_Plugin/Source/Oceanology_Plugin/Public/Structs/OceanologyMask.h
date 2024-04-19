#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyMask.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyMask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask", meta = (ClampMax = "1.0"), DisplayName="LandscapeVisibilityMask")
	double LandscapeVisibilityMask = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask", meta = (ClampMax = "1.0"), DisplayName="ObjectVisibilityMask")
	double ObjectVisibilityMask = 1.0;

	FOceanologyMask()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyMaskHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Mask")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyMask& Mask)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("LandscapeVisibilityMask", Mask.LandscapeVisibilityMask);
		MID->SetScalarParameterValue("ObjectVisibilityMask", Mask.ObjectVisibilityMask);
	}

	UFUNCTION(BlueprintPure, Category="Mask")
	static void LerpMask(
		const FOceanologyMask& A, 
		const FOceanologyMask& B, 
		const double Alpha, 
		FOceanologyMask& OutResult
	)
	{
		FOceanologyMask LocalResult;
		LocalResult.LandscapeVisibilityMask = UKismetMathLibrary::Lerp(A.LandscapeVisibilityMask, B.LandscapeVisibilityMask, Alpha);
		LocalResult.ObjectVisibilityMask = UKismetMathLibrary::Lerp(A.ObjectVisibilityMask, B.ObjectVisibilityMask, Alpha);
		OutResult = LocalResult;
	}
};
