#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyVolumetricFog.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyVolumetricFog
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VolumetricFog", DisplayName="MultiScatteringContribution")
	double MultiScatteringContribution = 2000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VolumetricFog", DisplayName="MultiScatteringOcclusion")
	double MultiScatteringOcclusion = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VolumetricFog", DisplayName="Wet Alpha")
	double WetAlpha = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VolumetricFog", DisplayName="ExponentialFogColor")
	FLinearColor ExponentialFogColor = FLinearColor(0.002, 0.02, 0.035, 1.0);

	FOceanologyVolumetricFog()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyVolumetricFogHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="VolumetricFog")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyVolumetricFog& VolumetricFog)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("MultiScatteringContribution", VolumetricFog.MultiScatteringContribution);
		MID->SetScalarParameterValue("MultiScatteringOcclusion", VolumetricFog.MultiScatteringOcclusion);
		MID->SetScalarParameterValue("Wet Alpha", VolumetricFog.WetAlpha);
		MID->SetVectorParameterValue("ExponentialFogColor", VolumetricFog.ExponentialFogColor);
	}

	UFUNCTION(BlueprintPure, Category="VolumetricFog")
	static void LerpVolumetricFog(
		const FOceanologyVolumetricFog& A, 
		const FOceanologyVolumetricFog& B, 
		const double Alpha, 
		FOceanologyVolumetricFog& OutResult
	)
	{
		FOceanologyVolumetricFog LocalResult;
		LocalResult.MultiScatteringContribution = UKismetMathLibrary::Lerp(A.MultiScatteringContribution, B.MultiScatteringContribution, Alpha);
		LocalResult.MultiScatteringOcclusion = UKismetMathLibrary::Lerp(A.MultiScatteringOcclusion, B.MultiScatteringOcclusion, Alpha);
		LocalResult.WetAlpha = UKismetMathLibrary::Lerp(A.WetAlpha, B.WetAlpha, Alpha);
		OutResult = LocalResult;
	}
};
