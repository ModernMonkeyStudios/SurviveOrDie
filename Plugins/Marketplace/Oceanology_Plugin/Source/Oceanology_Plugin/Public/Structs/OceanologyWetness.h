#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyWetness.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyWetness
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness", meta = (ClampMax = "20.0"), DisplayName="Band Opacity")
	double BandOpacity = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness", DisplayName="MoistureMetallic")
	double MoistureMetallic = 0.75;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness", DisplayName="MoistureRoughness")
	double MoistureRoughness = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness", DisplayName="Wet Hardness")
	double WetHardness = -10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness", DisplayName="Wet Location")
	double WetLocation = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness", DisplayName="Wet Radius")
	double WetRadius = 400.0;

	FOceanologyWetness()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyWetnessHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Wetness")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyWetness& Wetness)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Band Opacity", Wetness.BandOpacity);
		MID->SetScalarParameterValue("MoistureMetallic", Wetness.MoistureMetallic);
		MID->SetScalarParameterValue("MoistureRoughness", Wetness.MoistureRoughness);
		MID->SetScalarParameterValue("Wet Hardness", Wetness.WetHardness);
		MID->SetScalarParameterValue("Wet Location", Wetness.WetLocation);
		MID->SetScalarParameterValue("Wet Radius", Wetness.WetRadius);
	}

	UFUNCTION(BlueprintPure, Category="Wetness")
	static void LerpWetness(
		const FOceanologyWetness& A, 
		const FOceanologyWetness& B, 
		const double Alpha, 
		FOceanologyWetness& OutResult
	)
	{
		FOceanologyWetness LocalResult;
		LocalResult.BandOpacity = UKismetMathLibrary::Lerp(A.BandOpacity, B.BandOpacity, Alpha);
		LocalResult.MoistureMetallic = UKismetMathLibrary::Lerp(A.MoistureMetallic, B.MoistureMetallic, Alpha);
		LocalResult.MoistureRoughness = UKismetMathLibrary::Lerp(A.MoistureRoughness, B.MoistureRoughness, Alpha);
		LocalResult.WetHardness = UKismetMathLibrary::Lerp(A.WetHardness, B.WetHardness, Alpha);
		LocalResult.WetLocation = UKismetMathLibrary::Lerp(A.WetLocation, B.WetLocation, Alpha);
		LocalResult.WetRadius = UKismetMathLibrary::Lerp(A.WetRadius, B.WetRadius, Alpha);
		OutResult = LocalResult;
	}
};
