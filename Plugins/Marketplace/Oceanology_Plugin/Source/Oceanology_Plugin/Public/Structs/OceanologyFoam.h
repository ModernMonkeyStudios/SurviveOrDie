#pragma once

#include "Engine/Texture.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyFoam.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyFoam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="FoamDistortionIntensity")
	double FoamDistortionIntensity = 30.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="FoamDistortionScale")
	double FoamDistortionScale = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="FoamDistortionSpeed")
	double FoamDistortionSpeed = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="FoamNormals")
	double FoamNormals = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="UVBubbles")
	double UVBubbles = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="UVFoam")
	double UVFoam = 32.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="BubblesNormals")
	double BubblesNormals = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="T_BubblesColor")
	UTexture* T_BubblesColor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="T_BubblesNormals")
	UTexture* T_BubblesNormals = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="T_FoamColor")
	UTexture* T_FoamColor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", DisplayName="T_FoamNormals")
	UTexture* T_FoamNormals = nullptr;

	FOceanologyFoam()
	{
		T_BubblesColor = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_SeaFoam_5_.T_SeaFoam_5_"), nullptr, LOAD_None, nullptr);
		T_BubblesNormals = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_SeaFoam_6_.T_SeaFoam_6_"), nullptr, LOAD_None, nullptr);
		T_FoamColor = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_WaterFlow_01_Foam_Tiled.T_WaterFlow_01_Foam_Tiled"), nullptr, LOAD_None, nullptr);
		T_FoamNormals = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_WaterFlow_01_Foam_Tiled_N.T_WaterFlow_01_Foam_Tiled_N"), nullptr, LOAD_None, nullptr);
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyFoamHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Foam")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyFoam& Foam)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("FoamDistortionIntensity", Foam.FoamDistortionIntensity);
		MID->SetScalarParameterValue("FoamDistortionScale", Foam.FoamDistortionScale);
		MID->SetScalarParameterValue("FoamDistortionSpeed", Foam.FoamDistortionSpeed);
		MID->SetScalarParameterValue("FoamNormals", Foam.FoamNormals);
		MID->SetScalarParameterValue("UVBubbles", Foam.UVBubbles);
		MID->SetScalarParameterValue("UVFoam", Foam.UVFoam);
		MID->SetScalarParameterValue("BubblesNormals", Foam.BubblesNormals);
		MID->SetTextureParameterValue("T_BubblesColor", Foam.T_BubblesColor);
		MID->SetTextureParameterValue("T_BubblesNormals", Foam.T_BubblesNormals);
		MID->SetTextureParameterValue("T_FoamColor", Foam.T_FoamColor);
		MID->SetTextureParameterValue("T_FoamNormals", Foam.T_FoamNormals);
	}

	UFUNCTION(BlueprintPure, Category="Foam")
	static void LerpFoam(
		const FOceanologyFoam& A, 
		const FOceanologyFoam& B, 
		const double Alpha, 
		FOceanologyFoam& OutResult
	)
	{
		FOceanologyFoam LocalResult;
		LocalResult.FoamDistortionIntensity = UKismetMathLibrary::Lerp(A.FoamDistortionIntensity, B.FoamDistortionIntensity, Alpha);
		LocalResult.FoamDistortionScale = UKismetMathLibrary::Lerp(A.FoamDistortionScale, B.FoamDistortionScale, Alpha);
		LocalResult.FoamDistortionSpeed = UKismetMathLibrary::Lerp(A.FoamDistortionSpeed, B.FoamDistortionSpeed, Alpha);
		LocalResult.FoamNormals = UKismetMathLibrary::Lerp(A.FoamNormals, B.FoamNormals, Alpha);
		LocalResult.UVBubbles = UKismetMathLibrary::Lerp(A.UVBubbles, B.UVBubbles, Alpha);
		LocalResult.UVFoam = UKismetMathLibrary::Lerp(A.UVFoam, B.UVFoam, Alpha);
		LocalResult.BubblesNormals = UKismetMathLibrary::Lerp(A.BubblesNormals, B.BubblesNormals, Alpha);
		OutResult = LocalResult;
	}
};
