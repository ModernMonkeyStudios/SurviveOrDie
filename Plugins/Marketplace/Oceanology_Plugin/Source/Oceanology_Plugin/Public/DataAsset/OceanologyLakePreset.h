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
#include "Structs/OceanologyGroundCaustics.h"
#include "Structs/OceanologyLakePresetResult.h"
#include "OceanologyLakePreset.generated.h"

UCLASS(BlueprintType)
class OCEANOLOGY_PLUGIN_API UOceanologyLakePreset : public UOceanologyWaterPreset
{
	GENERATED_BODY()

public:
	UOceanologyLakePreset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caustics")
	FOceanologyGroundCaustics GroundCaustics;

	virtual void LoadPreset(AOceanologyWaterParent* Water, const TArray<FName> InclusionGroups) const override;

	UFUNCTION(BlueprintPure, Category="Lake Preset")
	static void LerpPreset(
		const UOceanologyLakePreset* A,
		const UOceanologyLakePreset* B,
		const double Alpha,
		FOceanologyWaterPresetResult& OutWaterPresetResult,
		FOceanologyLakePresetResult& OutPresetResult
	);
};
