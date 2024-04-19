// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums/OceanologyUnderwaterMode.h"
#include "Structs/OceanologyBubblesSettings.h"
#include "Structs/OceanologyCaustics.h"
#include "Structs/OceanologyFlipbook.h"
#include "Structs/OceanologyFoam.h"
#include "Structs/OceanologyFolding.h"
#include "Structs/OceanologyHorizonCorrection.h"
#include "Structs/OceanologyMask.h"
#include "Structs/OceanologyMaskUnderwater.h"
#include "Structs/OceanologyProcedural.h"
#include "Structs/OceanologyRefraction.h"
#include "Structs/OceanologyRVT.h"
#include "Structs/OceanologySurfaceScattering.h"
#include "Structs/OceanologyUnderwater.h"
#include "Structs/OceanologyVolumetricFog.h"
#include "Structs/OceanologyWaterPresetResult.h"
#include "Structs/OceanologyWaterProjection.h"
#include "Structs/OceanologyGGX.h"
#include "Enums/OceanologyGerstnerWaveSummarize.h"
#include "Structs/OceanologyBaseOffset.h"
#include "Structs/OceanologyGlobalDisplacement.h"
#include "Structs/OceanologyWaves.h"
#include "OceanologyWaterPreset.generated.h"

class AOceanologyWaterParent;

UCLASS(BlueprintType)
class OCEANOLOGY_PLUGIN_API UOceanologyWaterPreset : public UDataAsset
{
	GENERATED_BODY()

public:
	UOceanologyWaterPreset();

#if WITH_EDITOR
	FSimpleMulticastDelegate OnPropertyUpdated;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Global )")
	FOceanologyGlobalDisplacement GlobalDisplacement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )")
	FOceanologyBaseOffset BaseOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ1")
	FOceanologyWave_1 Wave_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ2")
	FOceanologyWave_2 Wave_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ3")
	FOceanologyWave_3 Wave_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ4")
	FOceanologyWave_4 Wave_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves")
	EOceanologyGerstnerWaveSummarize Summarize = EOceanologyGerstnerWaveSummarize::Waves_8;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater")
	EOceanologyUnderwaterMode UnderwaterMode = EOceanologyUnderwaterMode::Underwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater",
		meta = (EditCondition = "UnderwaterMode == EOceanologyUnderwaterMode::Underwater"))
	FOceanologyUnderwater Underwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater|God Rays",
		meta = (EditCondition = "UnderwaterMode == EOceanologyUnderwaterMode::VolumetricFog"))
	FOceanologyVolumetricFog VolumetricFog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater")
	FOceanologyMaskUnderwater MaskUnderwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Underwater|Bubbles Settings")
	FOceanologyBubblesSettings BubblesSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics")
	FOceanologyWaterProjection WaterProjection;

	virtual void LoadPreset(AOceanologyWaterParent* Water, const TArray<FName> InclusionGroups) const;

	static void LerpPreset(
		const UOceanologyWaterPreset* A,
		const UOceanologyWaterPreset* B,
		const double Alpha,
		FOceanologyWaterPresetResult& OutPresetResult
	);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
