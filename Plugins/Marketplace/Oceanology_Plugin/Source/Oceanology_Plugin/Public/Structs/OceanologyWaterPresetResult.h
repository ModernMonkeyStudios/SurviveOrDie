#pragma once

#include "Enums/OceanologyUnderwaterMode.h"
#include "Enums/OceanologyGerstnerWaveSummarize.h"
#include "OceanologyBaseOffset.h"
#include "OceanologyGlobalDisplacement.h"
#include "OceanologyWaves.h"
#include "OceanologyBubblesSettings.h"
#include "OceanologyCaustics.h"
#include "OceanologyFlipbook.h"
#include "OceanologyFoam.h"
#include "OceanologyFolding.h"
#include "OceanologyHorizonCorrection.h"
#include "OceanologyMask.h"
#include "OceanologyGGX.h"
#include "OceanologyMaskUnderwater.h"
#include "OceanologyProcedural.h"
#include "OceanologyRefraction.h"
#include "OceanologyRVT.h"
#include "OceanologySurfaceScattering.h"
#include "OceanologyUnderwater.h"
#include "OceanologyVolumetricFog.h"
#include "OceanologyWaterProjection.h"
#include "OceanologyWaterPresetResult.generated.h"

// has to be in SYNC with OceanologyWaterPreset fields
USTRUCT(BlueprintType)
struct FOceanologyWaterPresetResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves ( Global )")
	FOceanologyGlobalDisplacement GlobalDisplacement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves ( Custom )")
	FOceanologyBaseOffset BaseOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves ( Custom )", DisplayName="Σ1")
	FOceanologyWave_1 Wave_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves ( Custom )", DisplayName="Σ2")
	FOceanologyWave_2 Wave_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves ( Custom )", DisplayName="Σ3")
	FOceanologyWave_3 Wave_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves ( Custom )", DisplayName="Σ4")
	FOceanologyWave_4 Wave_4;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves")
	// EOceanologyGerstnerWaveSummarize Summarize = EOceanologyGerstnerWaveSummarize::Waves_8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Surface Scattering")
	FOceanologySurfaceScattering SurfaceScattering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Caustics")
	FOceanologyCaustics Caustics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Refraction")
	FOceanologyRefraction Refraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Normal Calculation")
	FOceanologyHorizonCorrection HorizonCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Normal Calculation")
	FOceanologyFlipbook Flipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam")
	FOceanologyFoam Foam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam|Folding")
	FOceanologyFolding Folding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural")
	FOceanologyProcedural Procedural;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT")
	FOceanologyRVT RVT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask")
	FOceanologyMask Mask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GGX")
	FOceanologyGGX GGX;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater")
	// EOceanologyUnderwaterMode UnderwaterMode = EOceanologyUnderwaterMode::Underwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater"/*,
		meta = (EditCondition = "UnderwaterMode == EOceanologyUnderwaterMode::Underwater")*/)
	FOceanologyUnderwater Underwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater|God Rays"/*,
		meta = (EditCondition = "UnderwaterMode == EOceanologyUnderwaterMode::VolumetricFog")*/)
	FOceanologyVolumetricFog VolumetricFog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater")
	FOceanologyMaskUnderwater MaskUnderwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Underwater|Bubbles Settings")
	FOceanologyBubblesSettings BubblesSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics")
	FOceanologyWaterProjection WaterProjection;
};
