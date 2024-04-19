// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Components/Wave/OceanologyGerstnerWaveSolverComponent.h"
#include "Actors/OceanologyInfiniteOceanActor.h"
#include "Actors/OceanologyLakeActor.h"
#include "Components/ActorComponent.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

void UOceanologyGerstnerWaveSolverComponent::InitWaves_Implementation(
	AOceanologyWaterParent* InOwner,
	const bool InUsingMPC_Waves
)
{
	Super::InitWaves_Implementation(InOwner, InUsingMPC_Waves);

	UpdateWaves();
}

void UOceanologyGerstnerWaveSolverComponent::UpdateWaves_Implementation()
{
	Super::UpdateWaves_Implementation();

	if (IsUsingMPC_Waves())
	{
		UpdateWavesByMPC();
	}
	else
	{
		UpdateWavesByMaterialParameters();
	}
}

void UOceanologyGerstnerWaveSolverComponent::UpdateWavesByMPC() const
{
	if (!Owner || !MPC_Waves)
	{
		return;
	}

	UMaterialParameterCollectionInstance* MPCI = GetWorld()->GetParameterCollectionInstance(MPC_Waves);
	if (!MPCI)
	{
		return;
	}

	UOceanologyGlobalDisplacementHelper::SetMaterialParameterCollections(MPCI, GlobalDisplacement);
	UOceanologyBaseOffsetHelper::SetMaterialParameterCollections(MPCI, BaseOffset);
	UOceanologyWavesHelper::SetMaterialParameterCollectionsWave_1(MPCI, Wave_1);
	UOceanologyWavesHelper::SetMaterialParameterCollectionsWave_2(MPCI, Wave_2);
	UOceanologyWavesHelper::SetMaterialParameterCollectionsWave_3(MPCI, Wave_3);
	UOceanologyWavesHelper::SetMaterialParameterCollectionsWave_4(MPCI, Wave_4);

	MPCI->SetVectorParameterValue(
		"Location Offset",
		FLinearColor(0, 0, Owner->DefaultSceneRoot->GetComponentLocation().Z, 1.0)
	);

	MPCI->SetScalarParameterValue(
		"CausticsScale",
		Owner->Caustics.CausticsScale
	);

	MPCI->SetScalarParameterValue(
		"LandscapeVisibility",
		Owner->Mask.LandscapeVisibilityMask
	);

	MPCI->SetScalarParameterValue(
		"WaterZMax",
		Owner->RVT.WaterZMax
	);

	MPCI->SetScalarParameterValue(
		"WaterZMin",
		Owner->RVT.WaterZMin
	);
}

void UOceanologyGerstnerWaveSolverComponent::UpdateWavesByMaterialParameters() const
{
	if (!Owner)
	{
		return;
	}

	UMaterialInstanceDynamic* WaterMID = Owner->GetWaterMID();
	if (!WaterMID)
	{
		return;
	}

	UOceanologyGlobalDisplacementHelper::SetMaterialParameters(WaterMID, GlobalDisplacement);
	UOceanologyBaseOffsetHelper::SetMaterialParameters(WaterMID, BaseOffset);
	UOceanologyWavesHelper::SetMaterialParametersWave_1(WaterMID, Wave_1);
	UOceanologyWavesHelper::SetMaterialParametersWave_2(WaterMID, Wave_2);
	UOceanologyWavesHelper::SetMaterialParametersWave_3(WaterMID, Wave_3);
	UOceanologyWavesHelper::SetMaterialParametersWave_4(WaterMID, Wave_4);

	if (UMaterialInstanceDynamic* CausticsMID = Owner->GetCausticsMID())
	{
		UOceanologyGlobalDisplacementHelper::SetMaterialParameters(CausticsMID, GlobalDisplacement);
		UOceanologyBaseOffsetHelper::SetMaterialParameters(CausticsMID, BaseOffset);
		UOceanologyWavesHelper::SetMaterialParametersWave_1(CausticsMID, Wave_1);
		UOceanologyWavesHelper::SetMaterialParametersWave_2(CausticsMID, Wave_2);
		UOceanologyWavesHelper::SetMaterialParametersWave_3(CausticsMID, Wave_3);
		UOceanologyWavesHelper::SetMaterialParametersWave_4(CausticsMID, Wave_4);
	}

	if (Owner->UnderwaterComponent && Owner->UnderwaterComponent->UnderwaterMode != EOceanologyUnderwaterMode::None)
	{
		UMaterialInstanceDynamic* UnderwaterMID = Owner->UnderwaterComponent->GetUnderwaterMID();
		UOceanologyGlobalDisplacementHelper::SetMaterialParameters(UnderwaterMID, GlobalDisplacement);
		UOceanologyBaseOffsetHelper::SetMaterialParameters(UnderwaterMID, BaseOffset);
		UOceanologyWavesHelper::SetMaterialParametersWave_1(UnderwaterMID, Wave_1);
		UOceanologyWavesHelper::SetMaterialParametersWave_2(UnderwaterMID, Wave_2);
		UOceanologyWavesHelper::SetMaterialParametersWave_3(UnderwaterMID, Wave_3);
		UOceanologyWavesHelper::SetMaterialParametersWave_4(UnderwaterMID, Wave_4);

		if (Owner->UnderwaterComponent->UnderwaterMode == EOceanologyUnderwaterMode::VolumetricFog)
		{
			UMaterialInstanceDynamic* VolumetricFogMID = Owner->UnderwaterComponent->GetVolumetricFogMID();
			UOceanologyGlobalDisplacementHelper::SetMaterialParameters(VolumetricFogMID, GlobalDisplacement);
			UOceanologyBaseOffsetHelper::SetMaterialParameters(VolumetricFogMID, BaseOffset);
			UOceanologyWavesHelper::SetMaterialParametersWave_1(VolumetricFogMID, Wave_1);
			UOceanologyWavesHelper::SetMaterialParametersWave_2(VolumetricFogMID, Wave_2);
			UOceanologyWavesHelper::SetMaterialParametersWave_3(VolumetricFogMID, Wave_3);
			UOceanologyWavesHelper::SetMaterialParametersWave_4(VolumetricFogMID, Wave_4);
		}
	}

	Owner->SetVectorParameterValue(
		"Location Offset",
		FLinearColor(0, 0, Owner->DefaultSceneRoot->GetComponentLocation().Z, 1.0)
	);

	Owner->SetScalarParameterValue(
		"CausticsScale",
		Owner->Caustics.CausticsScale
	);

	Owner->SetScalarParameterValue(
		"LandscapeVisibility",
		Owner->Mask.LandscapeVisibilityMask
	);

	Owner->SetScalarParameterValue(
		"WaterZMax",
		Owner->RVT.WaterZMax
	);

	Owner->SetScalarParameterValue(
		"WaterZMin",
		Owner->RVT.WaterZMin
	);
}

void UOceanologyGerstnerWaveSolverComponent::UpdateWavesByPresetResult_Implementation(
	AOceanologyWaterParent* Water,
	const FOceanologyWaterPresetResult& WaterPresetResult
)
{
	Super::UpdateWavesByPresetResult_Implementation(Water, WaterPresetResult);

	GlobalDisplacement = WaterPresetResult.GlobalDisplacement;
	BaseOffset = WaterPresetResult.BaseOffset;
	Wave_1 = WaterPresetResult.Wave_1;
	Wave_2 = WaterPresetResult.Wave_2;
	Wave_3 = WaterPresetResult.Wave_3;
	Wave_4 = WaterPresetResult.Wave_4;
	// Summarize=WaterPresetResult.Summarize;
}

void UOceanologyGerstnerWaveSolverComponent::UpdateLakeWavesByPresetResult_Implementation(
	const FOceanologyWaterPresetResult& WaterPresetResult,
	const FOceanologyLakePresetResult& LakePresetResult
)
{
	if (!Owner)
	{
		return;
	}

	AOceanologyLake* Lake = Cast<AOceanologyLake>(Owner);
	if (!Lake)
	{
		return;
	}

	UpdateWavesByPresetResult(Lake, WaterPresetResult);

	Lake->GroundCaustics = LakePresetResult.GroundCaustics;

	UOceanologyCausticsHelper::SetMaterialParameters(Lake->GetCausticsMID(), Lake->Caustics);
	UOceanologyGroundCausticsHelper::SetMaterialParameters(Lake->GetCausticsMID(), Lake->GroundCaustics);
	UOceanologyWaterProjectionHelper::SetMaterialParameters(Lake->GetCausticsMID(), Lake->WaterProjection);

	Lake->SetWaterMaterialParameters(Lake->GetWaterMID());

	UpdateWaves();
}

void UOceanologyGerstnerWaveSolverComponent::UpdateOceanWavesByPresetResult_Implementation(
	const FOceanologyWaterPresetResult& WaterPresetResult,
	const FOceanologyOceanPresetResult& OceanPresetResult
)
{
	if (!Owner)
	{
		return;
	}

	AOceanologyInfiniteOcean* Ocean = Cast<AOceanologyInfiniteOcean>(Owner);
	if (!Ocean)
	{
		return;
	}

	UpdateWavesByPresetResult(Ocean, WaterPresetResult);

	// EnableWetness=OceanPresetResult.EnableWetness;
	Ocean->Wetness = OceanPresetResult.Wetness;
	Ocean->SetWaterMaterialParameters(Ocean->GetWaterMID());

	UOceanologyWetnessHelper::SetMaterialParameters(Ocean->GetShorelineWetnessMID(), Ocean->Wetness);
	UOceanologyRVTHelper::SetMaterialParameters(Ocean->GetShorelineWetnessMID(), Ocean->RVT);

	UpdateWaves();
}

FVector UOceanologyGerstnerWaveSolverComponent::GetWaveHeightAtLocation_Implementation(const FVector& Location)
{
	constexpr double WaveNum = 4.0;
	const double WaveSize = GlobalDisplacement.SizeM * 0.05;
	const double WaveGlobalAmplitude = GlobalDisplacement.GlobalAmplitude * WaveSize;
	const double WaveOverallLength = GlobalDisplacement.OverallLength * WaveSize;
	const double WaveGlobalSpeed = GlobalDisplacement.GlobalSpeed * WaveSize;
	const FVector WaveBaseOffset = GerstnerWavesQuadruple(
		Location,
		WaveGlobalAmplitude * BaseOffset.Amplitude,
		BaseOffset.Steepness,
		WaveGlobalSpeed * BaseOffset.Speed,
		WaveOverallLength * BaseOffset.WaveLength,
		BaseOffset.Direction * GlobalDisplacement.GlobalWaveDirection,
		WaveNum,
		BaseOffset.Direction_Offset
	);
	const double WaveInterpolation = FMath::Clamp(
		((WaveBaseOffset.X + WaveBaseOffset.Y) / (WaveGlobalAmplitude * BaseOffset.Amplitude) * 0.5) * 0.01, 0.7,
		1.0);

	const FVector Wave_1_Vector = GerstnerWavesQuadruple(
		Location,
		(WaveInterpolation * (Wave_1.Amplitude_1 * WaveGlobalAmplitude)),
		(WaveInterpolation * Wave_1.Steepness_1 * GlobalDisplacement.Choppiness),
		WaveGlobalSpeed * Wave_1.Speed_1,
		WaveOverallLength * Wave_1.WaveLength_1,
		Wave_1.Direction_1 * GlobalDisplacement.GlobalWaveDirection,
		WaveNum,
		BaseOffset.Direction_Offset
	);

	const FVector Wave_2_Vector = GerstnerWavesQuadruple(
		Location,
		(WaveInterpolation * (Wave_2.Amplitude_2 * WaveGlobalAmplitude)),
		(WaveInterpolation * Wave_2.Steepness_2 * GlobalDisplacement.Choppiness),
		WaveGlobalSpeed * Wave_2.Speed_2,
		WaveOverallLength * Wave_2.WaveLength_2,
		Wave_2.Direction_2 * GlobalDisplacement.GlobalWaveDirection,
		WaveNum,
		BaseOffset.Direction_Offset2
	);

	const FVector Wave_3_Vector = GerstnerWavesQuadruple(
		Location,
		(WaveInterpolation * (Wave_3.Amplitude_3 * WaveGlobalAmplitude)),
		(WaveInterpolation * Wave_3.Steepness_3 * GlobalDisplacement.Choppiness),
		WaveGlobalSpeed * Wave_3.Speed_3,
		WaveOverallLength * Wave_3.WaveLength_3,
		Wave_3.Direction_3 * GlobalDisplacement.GlobalWaveDirection,
		WaveNum,
		BaseOffset.Direction_Offset2
	);

	const FVector Wave_4_Vector = GerstnerWavesQuadruple(
		Location,
		(WaveInterpolation * (Wave_4.Amplitude_4 * WaveGlobalAmplitude)),
		(WaveInterpolation * Wave_4.Steepness_4 * GlobalDisplacement.Choppiness),
		WaveGlobalSpeed * Wave_4.Speed_4,
		WaveOverallLength * Wave_4.WaveLength_4,
		Wave_4.Direction_4 * GlobalDisplacement.GlobalWaveDirection,
		WaveNum,
		BaseOffset.Direction_Offset2
	);

	FVector WaveHeight;

	switch (Summarize)
	{
	case EOceanologyGerstnerWaveSummarize::Waves_4:
		WaveHeight = WaveBaseOffset;
		break;

	case EOceanologyGerstnerWaveSummarize::Waves_8:
		WaveHeight = WaveBaseOffset + Wave_1_Vector;
		break;

	case EOceanologyGerstnerWaveSummarize::Waves_12:
		WaveHeight = WaveBaseOffset + Wave_1_Vector + Wave_2_Vector;
		break;

	case EOceanologyGerstnerWaveSummarize::Waves_16:
		WaveHeight = WaveBaseOffset + Wave_1_Vector + Wave_2_Vector + Wave_3_Vector;
		break;

	case EOceanologyGerstnerWaveSummarize::Waves_20:
		WaveHeight = WaveBaseOffset + Wave_1_Vector + Wave_2_Vector + Wave_3_Vector + Wave_4_Vector;
		break;
	default: ;
	}

	WaveHeight = Owner->GetActorLocation() + WaveHeight;

	return WaveHeight;
}

double UOceanologyGerstnerWaveSolverComponent::GetMaxWaveHeight_Implementation()
{
	return GlobalDisplacement.GlobalAmplitude
		+ BaseOffset.Amplitude
		+ Wave_1.Amplitude_1
		+ Wave_2.Amplitude_2
		+ Wave_3.Amplitude_3
		+ Wave_4.Amplitude_4;
}

double UOceanologyGerstnerWaveSolverComponent::GetGlobalWaveDirection_Implementation()
{
	return GlobalDisplacement.GlobalWaveDirection;
}

FVector UOceanologyGerstnerWaveSolverComponent::GerstnerWavesQuadruple(
	const FVector& Position,
	const double Amplitude,
	const double Steepness,
	const double Speed,
	const double WaveLength,
	const double Direction,
	const double NumWaves,
	const double DirectionOffset
)
{
	const FVector Wave_1_Vector = GerstnerWave(
		Position,
		Direction,
		Speed,
		WaveLength,
		Amplitude,
		Steepness,
		NumWaves * 4
	);

	const FVector Wave_2_Vector = GerstnerWave(
		Position,
		(Direction + (DirectionOffset * 0.1249)),
		Speed,
		WaveLength * 0.8,
		Amplitude * 0.8,
		Steepness,
		NumWaves * 4
	);

	const FVector Wave_3_Vector = GerstnerWave(
		Position,
		(Direction + (DirectionOffset * 0.24845)),
		Speed,
		WaveLength * 0.7,
		Amplitude * 0.7,
		Steepness,
		NumWaves * 4
	);

	const FVector Wave_4_Vector = GerstnerWave(
		Position,
		(Direction + (DirectionOffset * 0.52148)),
		Speed,
		WaveLength * 0.6,
		Amplitude * 0.6,
		Steepness,
		NumWaves * 4
	);

	return Wave_1_Vector + Wave_2_Vector + Wave_3_Vector + Wave_4_Vector;
}

FVector UOceanologyGerstnerWaveSolverComponent::GerstnerWave(
	const FVector& Position,
	const double Direction,
	const double Speed,
	const double WaveLength,
	const double Amplitude,
	const double Steepness,
	const double NumWaves
)
{
	const double WaveLengthW = DivideWave(WaveLength);
	const double SteepnessQ = MultiplyWave(Steepness);
	const double WA = (WaveLengthW * Amplitude);
	const double NumWaves0 = NumWaves * (PI * 2.0);
	const double QNumWaves = SteepnessQ / (NumWaves0 * WA);
	const double QA = QNumWaves * Amplitude;
	const FVector Direction0 = DirectionWave(Direction);
	const FVector QAD = FVector(QA) * Direction0;
	const FVector WD = WaveLengthW * Direction0;
	const double Dot = FVector::DotProduct(WD, Position);
	const double Phi = DivideWave(WaveLength) * Speed;
	const double TPhi = GameTimeInSeconds * Phi;
	const double DotTPhi = Dot + TPhi;
	const double Cosine = FMath::Cos(DotTPhi);
	const FVector XY = QAD * Cosine;
	const double Sine = FMath::Sin(DotTPhi);
	const double Z = Sine * Amplitude;

	return FVector(
		XY.X,
		XY.Y,
		Z
	);
}

double UOceanologyGerstnerWaveSolverComponent::DivideWave(const double Value)
{
	return (PI * 4.0) / Value;
}

double UOceanologyGerstnerWaveSolverComponent::MultiplyWave(const double Value)
{
	return (PI * 2) * Value;
}

FVector UOceanologyGerstnerWaveSolverComponent::DirectionWave(const double Direction)
{
	const double BaseValue = Direction * (PI * 2.0);

	return FVector(
		FMath::Sin(BaseValue),
		FMath::Cos(BaseValue),
		0
	);
}

#if WITH_EDITOR
void UOceanologyGerstnerWaveSolverComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, GlobalDisplacement)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, BaseOffset)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, Wave_1)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, Wave_2)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, Wave_3)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, Wave_4)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyGerstnerWaveSolverComponent, Summarize))
	{
		UpdateWaves();
	}
}
#endif

void UOceanologyGerstnerWaveSolverComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, GlobalDisplacement);
	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, BaseOffset);
	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, Wave_1);
	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, Wave_2);
	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, Wave_3);
	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, Wave_4);
	DOREPLIFETIME(UOceanologyGerstnerWaveSolverComponent, Summarize);
}
