#pragma once

#include "Engine/Texture.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyRVT.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyRVT
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", meta = (ClampMin = "0.02",ClampMax = "10.0"), DisplayName="Box Max Edge Falloff")
	double BoxMaxEdgeFalloff = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="Depth For DF Foam")
	double DepthForDFFoam = 128.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="Depth For Mask")
	double DepthForMask = 128.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="Depth For Still Shore")
	double DepthForStillShore = 128.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="Depth For Surface Scattering")
	double DepthForSurfaceScattering = 128.0;

	/** Lake */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="WaterZMax")
	double WaterZMax = 256.0;

	/** Lake */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="WaterZMin")
	double WaterZMin = 64.0;

	/** Determines the surface scattering color of the shore based on RVT shore texture. (T_Heightmap) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="Shoreline Surface Scattering Color")
	FLinearColor ShorelineSurfaceScatteringColor = FLinearColor(0.2, 0.5, 0.5, 2.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", DisplayName="T_Heightmap")
	UTexture* T_Heightmap = nullptr;

	FOceanologyRVT()
	{
		T_Heightmap = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_Debug.T_Debug"), nullptr, LOAD_None, nullptr);
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyRVTHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="RVT")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyRVT& RVT)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Box Max Edge Falloff", RVT.BoxMaxEdgeFalloff);
		MID->SetScalarParameterValue("Depth For DF Foam", RVT.DepthForDFFoam);
		MID->SetScalarParameterValue("Depth For Mask", RVT.DepthForMask);
		MID->SetScalarParameterValue("Depth For Still Shore", RVT.DepthForStillShore);
		MID->SetScalarParameterValue("Depth For Surface Scattering", RVT.DepthForSurfaceScattering);
		MID->SetScalarParameterValue("WaterZMax", RVT.WaterZMax);
		MID->SetScalarParameterValue("WaterZMin", RVT.WaterZMin);
		MID->SetVectorParameterValue("Shoreline Surface Scattering Color", RVT.ShorelineSurfaceScatteringColor);
		MID->SetTextureParameterValue("T_Heightmap", RVT.T_Heightmap);
	}

	UFUNCTION(BlueprintPure, Category="RVT")
	static void LerpRVT(
		const FOceanologyRVT& A, 
		const FOceanologyRVT& B, 
		const double Alpha, 
		FOceanologyRVT& OutResult
	)
	{
		FOceanologyRVT LocalResult;
		LocalResult.BoxMaxEdgeFalloff = UKismetMathLibrary::Lerp(A.BoxMaxEdgeFalloff, B.BoxMaxEdgeFalloff, Alpha);
		LocalResult.DepthForDFFoam = UKismetMathLibrary::Lerp(A.DepthForDFFoam, B.DepthForDFFoam, Alpha);
		LocalResult.DepthForMask = UKismetMathLibrary::Lerp(A.DepthForMask, B.DepthForMask, Alpha);
		LocalResult.DepthForStillShore = UKismetMathLibrary::Lerp(A.DepthForStillShore, B.DepthForStillShore, Alpha);
		LocalResult.DepthForSurfaceScattering = UKismetMathLibrary::Lerp(A.DepthForSurfaceScattering, B.DepthForSurfaceScattering, Alpha);
		LocalResult.WaterZMax = UKismetMathLibrary::Lerp(A.WaterZMax, B.WaterZMax, Alpha);
		LocalResult.WaterZMin = UKismetMathLibrary::Lerp(A.WaterZMin, B.WaterZMin, Alpha);
		OutResult = LocalResult;
	}
};
