#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyFolding.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyFolding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamBoost")
	double FoamBoost = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamContrast")
	double FoamContrast = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamDepthMin")
	double FoamDepthMin = 500.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamOpacity")
	double FoamOpacity = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamPower")
	double FoamPower = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamRoughness")
	double FoamRoughness = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamScatterBias")
	double FoamScatterBias = 0.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamTextureBlendMin")
	double FoamTextureBlendMin = 0.4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamTextureBlendWidth")
	double FoamTextureBlendWidth = 0.4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="HeightBias")
	double HeightBias = -1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="ScatteredLuminance")
	double ScatteredLuminance = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="SmoothMax")
	double SmoothMax = 1900.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="SmoothMin")
	double SmoothMin = -2000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamEmissiveHigh")
	FLinearColor FoamEmissiveHigh = FLinearColor(20.0, 10.0, 5.0, 0.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamEmissiveLow")
	FLinearColor FoamEmissiveLow = FLinearColor(5.0, 2.5, 1.25, 0.0);

	/** Determines the color of the foam. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Folding", DisplayName="FoamScattering")
	FLinearColor FoamScattering = FLinearColor(0.2, 0.5, 0.5, 2.0);

	FOceanologyFolding()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyFoldingHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Folding")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyFolding& Folding)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("FoamBoost", Folding.FoamBoost);
		MID->SetScalarParameterValue("FoamContrast", Folding.FoamContrast);
		MID->SetScalarParameterValue("FoamDepthMin", Folding.FoamDepthMin);
		MID->SetScalarParameterValue("FoamOpacity", Folding.FoamOpacity);
		MID->SetScalarParameterValue("FoamPower", Folding.FoamPower);
		MID->SetScalarParameterValue("FoamRoughness", Folding.FoamRoughness);
		MID->SetScalarParameterValue("FoamScatterBias", Folding.FoamScatterBias);
		MID->SetScalarParameterValue("FoamTextureBlendMin", Folding.FoamTextureBlendMin);
		MID->SetScalarParameterValue("FoamTextureBlendWidth", Folding.FoamTextureBlendWidth);
		MID->SetScalarParameterValue("HeightBias", Folding.HeightBias);
		MID->SetScalarParameterValue("ScatteredLuminance", Folding.ScatteredLuminance);
		MID->SetScalarParameterValue("SmoothMax", Folding.SmoothMax);
		MID->SetScalarParameterValue("SmoothMin", Folding.SmoothMin);
		MID->SetVectorParameterValue("FoamEmissiveHigh", Folding.FoamEmissiveHigh);
		MID->SetVectorParameterValue("FoamEmissiveLow", Folding.FoamEmissiveLow);
		MID->SetVectorParameterValue("FoamScattering", Folding.FoamScattering);
	}

	UFUNCTION(BlueprintPure, Category="Folding")
	static void LerpFolding(
		const FOceanologyFolding& A, 
		const FOceanologyFolding& B, 
		const double Alpha, 
		FOceanologyFolding& OutResult
	)
	{
		FOceanologyFolding LocalResult;
		LocalResult.FoamBoost = UKismetMathLibrary::Lerp(A.FoamBoost, B.FoamBoost, Alpha);
		LocalResult.FoamContrast = UKismetMathLibrary::Lerp(A.FoamContrast, B.FoamContrast, Alpha);
		LocalResult.FoamDepthMin = UKismetMathLibrary::Lerp(A.FoamDepthMin, B.FoamDepthMin, Alpha);
		LocalResult.FoamOpacity = UKismetMathLibrary::Lerp(A.FoamOpacity, B.FoamOpacity, Alpha);
		LocalResult.FoamPower = UKismetMathLibrary::Lerp(A.FoamPower, B.FoamPower, Alpha);
		LocalResult.FoamRoughness = UKismetMathLibrary::Lerp(A.FoamRoughness, B.FoamRoughness, Alpha);
		LocalResult.FoamScatterBias = UKismetMathLibrary::Lerp(A.FoamScatterBias, B.FoamScatterBias, Alpha);
		LocalResult.FoamTextureBlendMin = UKismetMathLibrary::Lerp(A.FoamTextureBlendMin, B.FoamTextureBlendMin, Alpha);
		LocalResult.FoamTextureBlendWidth = UKismetMathLibrary::Lerp(A.FoamTextureBlendWidth, B.FoamTextureBlendWidth, Alpha);
		LocalResult.HeightBias = UKismetMathLibrary::Lerp(A.HeightBias, B.HeightBias, Alpha);
		LocalResult.ScatteredLuminance = UKismetMathLibrary::Lerp(A.ScatteredLuminance, B.ScatteredLuminance, Alpha);
		LocalResult.SmoothMax = UKismetMathLibrary::Lerp(A.SmoothMax, B.SmoothMax, Alpha);
		LocalResult.SmoothMin = UKismetMathLibrary::Lerp(A.SmoothMin, B.SmoothMin, Alpha);
		OutResult = LocalResult;
	}
};
