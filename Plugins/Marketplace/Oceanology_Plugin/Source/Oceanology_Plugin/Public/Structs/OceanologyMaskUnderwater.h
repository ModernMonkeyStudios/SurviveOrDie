#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyMaskUnderwater.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyMaskUnderwater
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MaskUnderwater", DisplayName="UnderwaterLandscapeMask")
	double UnderwaterLandscapeMask = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MaskUnderwater", DisplayName="UnderwaterObjectMask")
	double UnderwaterObjectMask = 1.0;

	FOceanologyMaskUnderwater()
	{
	}
};

UCLASS()
class UOceanologyMaskUnderwaterHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="MaskUnderwater")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyMaskUnderwater& MaskUnderwater)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("UnderwaterLandscapeMask", MaskUnderwater.UnderwaterLandscapeMask);
		MID->SetScalarParameterValue("UnderwaterObjectMask", MaskUnderwater.UnderwaterObjectMask);
	}

	UFUNCTION(BlueprintPure, Category="MaskUnderwater")
	static void LerpMaskUnderwater(
		const FOceanologyMaskUnderwater& A, 
		const FOceanologyMaskUnderwater& B, 
		const double Alpha, 
		FOceanologyMaskUnderwater& OutResult
	)
	{
		FOceanologyMaskUnderwater LocalResult;
		LocalResult.UnderwaterLandscapeMask = UKismetMathLibrary::Lerp(A.UnderwaterLandscapeMask, B.UnderwaterLandscapeMask, Alpha);
		LocalResult.UnderwaterObjectMask = UKismetMathLibrary::Lerp(A.UnderwaterObjectMask, B.UnderwaterObjectMask, Alpha);
		OutResult = LocalResult;
	}
};
