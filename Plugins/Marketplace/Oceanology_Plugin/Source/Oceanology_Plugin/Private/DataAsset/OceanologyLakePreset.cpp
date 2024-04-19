// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "DataAsset/OceanologyLakePreset.h"
#include "Actors/OceanologyLakeActor.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UOceanologyLakePreset::UOceanologyLakePreset()
{
}

void UOceanologyLakePreset::LoadPreset(
	AOceanologyWaterParent* Water,
	const TArray<FName> InclusionGroups
) const
{
	Super::LoadPreset(Water, InclusionGroups);

	if (!Water->IsA(AOceanologyLake::StaticClass()))
	{
		return;
	}

	AOceanologyLake* Lake = Cast<AOceanologyLake>(Water);

	if (InclusionGroups.Contains("GroundCaustics"))
	{
		Lake->GroundCaustics = GroundCaustics;
	}
}

void UOceanologyLakePreset::LerpPreset(
	const UOceanologyLakePreset* A,
	const UOceanologyLakePreset* B,
	const double Alpha,
	FOceanologyWaterPresetResult& OutWaterPresetResult,
	FOceanologyLakePresetResult& OutPresetResult
)
{
	FOceanologyLakePresetResult LocalLakePresetResult;

	//@formatter:off
	UOceanologyGroundCausticsHelper::LerpGroundCaustics(A->GroundCaustics, B->GroundCaustics, Alpha, LocalLakePresetResult.GroundCaustics);
	//@formatter:on

	UOceanologyWaterPreset::LerpPreset(A, B, Alpha, OutWaterPresetResult);
	OutPresetResult = LocalLakePresetResult;
}
