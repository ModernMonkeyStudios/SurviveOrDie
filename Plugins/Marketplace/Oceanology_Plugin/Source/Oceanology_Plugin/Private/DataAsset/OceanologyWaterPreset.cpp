// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "DataAsset/OceanologyWaterPreset.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Components/Wave/OceanologyGerstnerWaveSolverComponent.h"

UOceanologyWaterPreset::UOceanologyWaterPreset()
{
}

void UOceanologyWaterPreset::LoadPreset(
	AOceanologyWaterParent* Water,
	const TArray<FName> InclusionGroups
) const
{
	if (Water->GetWaveSolver()->IsA(UOceanologyGerstnerWaveSolverComponent::StaticClass()))
	{
		UOceanologyGerstnerWaveSolverComponent* GerstnerWaveSolver = Cast<UOceanologyGerstnerWaveSolverComponent>(
			Water->GetWaveSolver()
		);

		if (InclusionGroups.Contains("GlobalDisplacement"))
		{
			GerstnerWaveSolver->GlobalDisplacement = GlobalDisplacement;
		}

		if (InclusionGroups.Contains("BaseOffset"))
		{
			GerstnerWaveSolver->BaseOffset = BaseOffset;
		}

		if (InclusionGroups.Contains("Wave_1"))
		{
			GerstnerWaveSolver->Wave_1 = Wave_1;
		}

		if (InclusionGroups.Contains("Wave_2"))
		{
			GerstnerWaveSolver->Wave_2 = Wave_2;
		}

		if (InclusionGroups.Contains("Wave_3"))
		{
			GerstnerWaveSolver->Wave_3 = Wave_3;
		}

		if (InclusionGroups.Contains("Wave_4"))
		{
			GerstnerWaveSolver->Wave_4 = Wave_4;
		}

		if (InclusionGroups.Contains("Summarize"))
		{
			GerstnerWaveSolver->Summarize = Summarize;
		}
	}

	// Water Parent
	if (InclusionGroups.Contains("SurfaceScattering"))
	{
		Water->SurfaceScattering = SurfaceScattering;
	}

	if (InclusionGroups.Contains("Caustics"))
	{
		Water->Caustics = Caustics;
	}

	if (InclusionGroups.Contains("Refraction"))
	{
		Water->Refraction = Refraction;
	}

	if (InclusionGroups.Contains("HorizonCorrection"))
	{
		Water->HorizonCorrection = HorizonCorrection;
	}

	if (InclusionGroups.Contains("Flipbook"))
	{
		Water->Flipbook = Flipbook;
	}

	if (InclusionGroups.Contains("Foam"))
	{
		Water->Foam = Foam;
	}

	if (InclusionGroups.Contains("Folding"))
	{
		Water->Folding = Folding;
	}

	if (InclusionGroups.Contains("Procedural"))
	{
		Water->Procedural = Procedural;
	}

	if (InclusionGroups.Contains("RVT"))
	{
		Water->RVT = RVT;
	}

	if (InclusionGroups.Contains("Mask"))
	{
		Water->Mask = Mask;
	}

	if (InclusionGroups.Contains("GGX"))
	{
		Water->GGX = GGX;
	}

	// Underwater
	if (InclusionGroups.Contains("UnderwaterMode"))
	{
		Water->UnderwaterComponent->UnderwaterMode = UnderwaterMode;
	}

	if (InclusionGroups.Contains("Underwater"))
	{
		Water->UnderwaterComponent->Underwater = Underwater;
	}

	if (InclusionGroups.Contains("VolumetricFog"))
	{
		Water->UnderwaterComponent->VolumetricFog = VolumetricFog;
	}

	if (InclusionGroups.Contains("MaskUnderwater"))
	{
		Water->UnderwaterComponent->MaskUnderwater = MaskUnderwater;
	}

	if (InclusionGroups.Contains("BubblesSettings"))
	{
		Water->UnderwaterComponent->BubblesSettings = BubblesSettings;
	}

	if (InclusionGroups.Contains("WaterProjection"))
	{
		Water->UnderwaterComponent->WaterProjection = WaterProjection;
	}
}

void UOceanologyWaterPreset::LerpPreset(
	const UOceanologyWaterPreset* A,
	const UOceanologyWaterPreset* B,
	const double Alpha,
	FOceanologyWaterPresetResult& OutPresetResult
)
{
	FOceanologyWaterPresetResult LocalWaterPresetResult;

	//@formatter:off

	UOceanologyGlobalDisplacementHelper::LerpGlobalDisplacement(A->GlobalDisplacement,B->GlobalDisplacement, Alpha, LocalWaterPresetResult.GlobalDisplacement);
	UOceanologyBaseOffsetHelper::LerpBaseOffset(A->BaseOffset,B->BaseOffset, Alpha, LocalWaterPresetResult.BaseOffset);
	UOceanologyWavesHelper::LerpWave_1(A->Wave_1,B->Wave_1, Alpha, LocalWaterPresetResult.Wave_1);
	UOceanologyWavesHelper::LerpWave_2(A->Wave_2,B->Wave_2, Alpha, LocalWaterPresetResult.Wave_2);
	UOceanologyWavesHelper::LerpWave_3(A->Wave_3,B->Wave_3, Alpha, LocalWaterPresetResult.Wave_3);
	UOceanologyWavesHelper::LerpWave_4(A->Wave_4,B->Wave_4, Alpha, LocalWaterPresetResult.Wave_4);
	
	UOceanologySurfaceScatteringHelper::LerpSurfaceScattering(A->SurfaceScattering,B->SurfaceScattering, Alpha, LocalWaterPresetResult.SurfaceScattering);
	UOceanologyCausticsHelper::LerpCaustics(A->Caustics,B->Caustics, Alpha, LocalWaterPresetResult.Caustics);
	UOceanologyRefractionHelper::LerpRefraction(A->Refraction,B->Refraction, Alpha, LocalWaterPresetResult.Refraction);
	UOceanologyHorizonCorrectionHelper::LerpHorizonCorrection(A->HorizonCorrection,B->HorizonCorrection, Alpha, LocalWaterPresetResult.HorizonCorrection);
	UOceanologyFlipbookHelper::LerpFlipbook(A->Flipbook,B->Flipbook, Alpha, LocalWaterPresetResult.Flipbook);
	UOceanologyFoamHelper::LerpFoam(A->Foam,B->Foam, Alpha, LocalWaterPresetResult.Foam);
	UOceanologyFoldingHelper::LerpFolding(A->Folding,B->Folding, Alpha, LocalWaterPresetResult.Folding);
	UOceanologyProceduralHelper::LerpProcedural(A->Procedural,B->Procedural, Alpha, LocalWaterPresetResult.Procedural);
	UOceanologyRVTHelper::LerpRVT(A->RVT,B->RVT, Alpha, LocalWaterPresetResult.RVT);
	UOceanologyMaskHelper::LerpMask(A->Mask,B->Mask, Alpha, LocalWaterPresetResult.Mask);
	// Lerp(A->UnderwaterMode,B->UnderwaterMode, Alpha, LocalWaterPresetResult.UnderwaterMode);
	UOceanologyUnderwaterHelper::LerpUnderwater(A->Underwater,B->Underwater, Alpha, LocalWaterPresetResult.Underwater);
	UOceanologyVolumetricFogHelper::LerpVolumetricFog(A->VolumetricFog,B->VolumetricFog, Alpha, LocalWaterPresetResult.VolumetricFog);
	UOceanologyMaskUnderwaterHelper::LerpMaskUnderwater(A->MaskUnderwater,B->MaskUnderwater, Alpha, LocalWaterPresetResult.MaskUnderwater);
	// Lerp(A->BubblesSettings,B->BubblesSettings, Alpha, LocalWaterPresetResult.BubblesSettings);
	UOceanologyWaterProjectionHelper::LerpWaterProjection(A->WaterProjection,B->WaterProjection, Alpha, LocalWaterPresetResult.WaterProjection);
	UOceanologyGGXHelper::LerpGGX(A->GGX, B->GGX, Alpha, LocalWaterPresetResult.GGX);
	//@formatter:on

	OutPresetResult = LocalWaterPresetResult;
}

#if WITH_EDITOR
void UOceanologyWaterPreset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnPropertyUpdated.Broadcast();
}
#endif
