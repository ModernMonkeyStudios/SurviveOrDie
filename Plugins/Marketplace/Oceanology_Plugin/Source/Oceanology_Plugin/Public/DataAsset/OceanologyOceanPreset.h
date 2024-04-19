// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "OceanologyWaterPreset.h"
#include "Structs/OceanologyOceanPresetResult.h"
#include "Structs/OceanologyWetness.h"
#include "OceanologyOceanPreset.generated.h"

UCLASS(BlueprintType)
class OCEANOLOGY_PLUGIN_API UOceanologyOceanPreset : public UOceanologyWaterPreset
{
	GENERATED_BODY()

public:
	UOceanologyOceanPreset();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness")
	bool EnableWetness = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness")
	FOceanologyWetness Wetness;

	virtual void LoadPreset(AOceanologyWaterParent* Water, const TArray<FName> InclusionGroups) const override;

	UFUNCTION(BlueprintPure, Category="Ocean Preset")
	static void LerpPreset(
		const UOceanologyOceanPreset* A,
		const UOceanologyOceanPreset* B,
		const double Alpha,
		FOceanologyWaterPresetResult& OutWaterPresetResult,
		FOceanologyOceanPresetResult& OutOceanPresetResult
	);
	
};
