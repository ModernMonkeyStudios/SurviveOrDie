#pragma once

#include "Engine/Texture.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyHorizonCorrection.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyHorizonCorrection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HorizonCorrection", DisplayName="DetailNormalSpeed")
	double DetailNormalSpeed = -0.015;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HorizonCorrection", DisplayName="DetailScaleNormal")
	double DetailScaleNormal = 32768.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HorizonCorrection", DisplayName="DistantNormalLength")
	double DistantNormalLength = 80000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HorizonCorrection", DisplayName="DistantNormalOffset")
	double DistantNormalOffset = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HorizonCorrection", DisplayName="HorizonCorrectionNormalIntensity")
	double HorizonCorrectionNormalIntensity = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HorizonCorrection", DisplayName="T_DetailNormal")
	UTexture* T_DetailNormal = nullptr;

	FOceanologyHorizonCorrection()
	{
		T_DetailNormal = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_Ocean.T_Ocean"), nullptr, LOAD_None, nullptr);
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyHorizonCorrectionHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="HorizonCorrection")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyHorizonCorrection& HorizonCorrection)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("DetailNormalSpeed", HorizonCorrection.DetailNormalSpeed);
		MID->SetScalarParameterValue("DetailScaleNormal", HorizonCorrection.DetailScaleNormal);
		MID->SetScalarParameterValue("DistantNormalLength", HorizonCorrection.DistantNormalLength);
		MID->SetScalarParameterValue("DistantNormalOffset", HorizonCorrection.DistantNormalOffset);
		MID->SetScalarParameterValue("HorizonCorrectionNormalIntensity", HorizonCorrection.HorizonCorrectionNormalIntensity);
		MID->SetTextureParameterValue("T_DetailNormal", HorizonCorrection.T_DetailNormal);
	}

	UFUNCTION(BlueprintPure, Category="HorizonCorrection")
	static void LerpHorizonCorrection(
		const FOceanologyHorizonCorrection& A, 
		const FOceanologyHorizonCorrection& B, 
		const double Alpha, 
		FOceanologyHorizonCorrection& OutResult
	)
	{
		FOceanologyHorizonCorrection LocalResult;
		LocalResult.DetailNormalSpeed = UKismetMathLibrary::Lerp(A.DetailNormalSpeed, B.DetailNormalSpeed, Alpha);
		LocalResult.DetailScaleNormal = UKismetMathLibrary::Lerp(A.DetailScaleNormal, B.DetailScaleNormal, Alpha);
		LocalResult.DistantNormalLength = UKismetMathLibrary::Lerp(A.DistantNormalLength, B.DistantNormalLength, Alpha);
		LocalResult.DistantNormalOffset = UKismetMathLibrary::Lerp(A.DistantNormalOffset, B.DistantNormalOffset, Alpha);
		LocalResult.HorizonCorrectionNormalIntensity = UKismetMathLibrary::Lerp(A.HorizonCorrectionNormalIntensity, B.HorizonCorrectionNormalIntensity, Alpha);
		OutResult = LocalResult;
	}
};
