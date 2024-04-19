#pragma once

#include "Engine/Texture.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyFlipbook.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyFlipbook
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="DistantWaterScale")
	double DistantWaterScale = 512.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="FarNormalFresnelPower")
	double FarNormalFresnelPower = 9.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="FrameRateFar")
	double FrameRateFar = -10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="FrameRateNear")
	double FrameRateNear = 30.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="NearNormalStrength")
	double NearNormalStrength = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="NearWaterScale")
	double NearWaterScale = 32.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="NormalLongDistanceStrength")
	double NormalLongDistanceStrength = 0.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="NormalMidDistanceForce")
	double NormalMidDistanceForce = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="T_FarNormalWater")
	UTexture* T_FarNormalWater = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flipbook", DisplayName="T_WaterNormals")
	UTexture* T_WaterNormals = nullptr;

	FOceanologyFlipbook()
	{
		T_FarNormalWater = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_FFTNormal01.T_FFTNormal01"), nullptr, LOAD_None, nullptr);
		T_WaterNormals = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_FFTNormal02.T_FFTNormal02"), nullptr, LOAD_None, nullptr);
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyFlipbookHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Flipbook")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyFlipbook& Flipbook)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("DistantWaterScale", Flipbook.DistantWaterScale);
		MID->SetScalarParameterValue("FarNormalFresnelPower", Flipbook.FarNormalFresnelPower);
		MID->SetScalarParameterValue("FrameRateFar", Flipbook.FrameRateFar);
		MID->SetScalarParameterValue("FrameRateNear", Flipbook.FrameRateNear);
		MID->SetScalarParameterValue("NearNormalStrength", Flipbook.NearNormalStrength);
		MID->SetScalarParameterValue("NearWaterScale", Flipbook.NearWaterScale);
		MID->SetScalarParameterValue("NormalLongDistanceStrength", Flipbook.NormalLongDistanceStrength);
		MID->SetScalarParameterValue("NormalMidDistanceForce", Flipbook.NormalMidDistanceForce);
		MID->SetTextureParameterValue("T_FarNormalWater", Flipbook.T_FarNormalWater);
		MID->SetTextureParameterValue("T_WaterNormals", Flipbook.T_WaterNormals);
	}

	UFUNCTION(BlueprintPure, Category="Flipbook")
	static void LerpFlipbook(
		const FOceanologyFlipbook& A, 
		const FOceanologyFlipbook& B, 
		const double Alpha, 
		FOceanologyFlipbook& OutResult
	)
	{
		FOceanologyFlipbook LocalResult;
		LocalResult.DistantWaterScale = UKismetMathLibrary::Lerp(A.DistantWaterScale, B.DistantWaterScale, Alpha);
		LocalResult.FarNormalFresnelPower = UKismetMathLibrary::Lerp(A.FarNormalFresnelPower, B.FarNormalFresnelPower, Alpha);
		LocalResult.FrameRateFar = UKismetMathLibrary::Lerp(A.FrameRateFar, B.FrameRateFar, Alpha);
		LocalResult.FrameRateNear = UKismetMathLibrary::Lerp(A.FrameRateNear, B.FrameRateNear, Alpha);
		LocalResult.NearNormalStrength = UKismetMathLibrary::Lerp(A.NearNormalStrength, B.NearNormalStrength, Alpha);
		LocalResult.NearWaterScale = UKismetMathLibrary::Lerp(A.NearWaterScale, B.NearWaterScale, Alpha);
		LocalResult.NormalLongDistanceStrength = UKismetMathLibrary::Lerp(A.NormalLongDistanceStrength, B.NormalLongDistanceStrength, Alpha);
		LocalResult.NormalMidDistanceForce = UKismetMathLibrary::Lerp(A.NormalMidDistanceForce, B.NormalMidDistanceForce, Alpha);
		OutResult = LocalResult;
	}
};
