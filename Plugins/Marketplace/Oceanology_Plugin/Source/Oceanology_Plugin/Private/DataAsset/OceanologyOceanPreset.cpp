// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "DataAsset/OceanologyOceanPreset.h"
#include "Actors/OceanologyInfiniteOceanActor.h"
#include "Components/Wave/OceanologyGerstnerWaveSolverComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UOceanologyOceanPreset::UOceanologyOceanPreset()
{
}

void UOceanologyOceanPreset::LoadPreset(
	AOceanologyWaterParent* Water,
	const TArray<FName> InclusionGroups
) const
{
	Super::LoadPreset(Water, InclusionGroups);

	if (!Water->IsA(AOceanologyInfiniteOcean::StaticClass()))
	{
		return;
	}

	AOceanologyInfiniteOcean* Ocean = Cast<AOceanologyInfiniteOcean>(Water);
	if (InclusionGroups.Contains("EnableWetness"))
	{
		Ocean->EnableWetness = EnableWetness;
	}

	if (InclusionGroups.Contains("Wetness"))
	{
		Ocean->Wetness = Wetness;
	}
}

void UOceanologyOceanPreset::LerpPreset(
	const UOceanologyOceanPreset* A,
	const UOceanologyOceanPreset* B,
	const double Alpha,
	FOceanologyWaterPresetResult& OutWaterPresetResult,
	FOceanologyOceanPresetResult& OutOceanPresetResult
)
{
	FOceanologyOceanPresetResult LocalOceanPresetResult;

	//@formatter:off
	// Lerp(A->Summarize,B->Summarize, Alpha, LocalOceanPresetResult.Summarize);
	// Lerp(A->EnableWetness,B->EnableWetness, Alpha, LocalOceanPresetResult.EnableWetness);
	UOceanologyWetnessHelper::LerpWetness(A->Wetness,B->Wetness, Alpha, LocalOceanPresetResult.Wetness);
	//@formatter:on

	UOceanologyWaterPreset::LerpPreset(A, B, Alpha, OutWaterPresetResult);
	OutOceanPresetResult = LocalOceanPresetResult;
}
