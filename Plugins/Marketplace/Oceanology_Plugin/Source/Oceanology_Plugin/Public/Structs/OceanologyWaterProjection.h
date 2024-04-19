#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyWaterProjection.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyWaterProjection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WaterProjection", DisplayName="VirtualRealityEyeOffset")
	double VirtualRealityEyeOffset = 0.5;

	FOceanologyWaterProjection()
	{
	}
};

UCLASS()
class UOceanologyWaterProjectionHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="WaterProjection")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyWaterProjection& WaterProjection)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("VirtualRealityEyeOffset", WaterProjection.VirtualRealityEyeOffset);
	}

	UFUNCTION(BlueprintPure, Category="WaterProjection")
	static void LerpWaterProjection(
		const FOceanologyWaterProjection& A, 
		const FOceanologyWaterProjection& B, 
		const double Alpha, 
		FOceanologyWaterProjection& OutResult
	)
	{
		FOceanologyWaterProjection LocalResult;
		LocalResult.VirtualRealityEyeOffset = UKismetMathLibrary::Lerp(A.VirtualRealityEyeOffset, B.VirtualRealityEyeOffset, Alpha);
		OutResult = LocalResult;
	}
};
