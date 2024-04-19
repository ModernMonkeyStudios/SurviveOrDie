// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Actors/OceanologyWaterNiagaraWavesVolume.h"
#include "NiagaraComponent.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/BrushComponent.h"
#include "Components/Wave/OceanologyGerstnerWaveSolverComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Structs/OceanologyGlobalDisplacement.h"
#include "Utils/OceanologyMessageUtils.h"

AOceanologyWaterNiagaraWavesVolume::AOceanologyWaterNiagaraWavesVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false; // Visual effects must NOT replicate.
	bEnableAutoLODGeneration = false;
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif

	NiagaraWavesComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraWaves"));
	NiagaraWavesComponent->SetupAttachment(GetBrushComponent());
}

void AOceanologyWaterNiagaraWavesVolume::ReloadSettings()
{
	Init();

	UOceanologyMessageUtils::CreateMessage(this, "Settings reloaded successfully!", true);
}

void AOceanologyWaterNiagaraWavesVolume::DebugBounds()
{
	const FVector Origin = GetBrushComponent()->Bounds.Origin;
	const FVector BoxExtent = GetBrushComponent()->Bounds.BoxExtent;

	UKismetSystemLibrary::DrawDebugBox(this, Origin, BoxExtent, FLinearColor::Green, GetActorRotation(), 1.0, 10.0);
}

void AOceanologyWaterNiagaraWavesVolume::Init()
{
	if (!OceanologyWater)
	{
		UOceanologyMessageUtils::CreateMessage(
			this,
			"OceanologyWater is not set! The water volume will NOT work!",
			false
		);
		return;
	}

	UOceanologyWaveSolverComponent* WaveSolver = OceanologyWater->GetWaveSolver();
	if (!WaveSolver
		|| !WaveSolver->IsA(UOceanologyGerstnerWaveSolverComponent::StaticClass()))
	{
		UOceanologyMessageUtils::CreateMessage(
			this,
			"WaveSolver is not configured properly!",
			false
		);
		return;
	}

	if (!NiagaraWaves)
	{
		NiagaraWaves = GetDefault<UOceanologyRuntimeSettings>()->GetNiagaraGerstnerWavesEffect();
	}

	NiagaraWavesComponent->SetAsset(NiagaraWaves);
	NiagaraWavesComponent->SetVariableInt("AreaBoundsScaleX", (GetBrushComponent()->Bounds.BoxExtent.X * 2) / 10);
	NiagaraWavesComponent->SetVariableInt("AreaBoundsScaleY", (GetBrushComponent()->Bounds.BoxExtent.Y * 2) / 10);

	UOceanologyGerstnerWaveSolverComponent* GerstnerWaveSolver = Cast<UOceanologyGerstnerWaveSolverComponent>(
		WaveSolver
	);

	if (!GerstnerWaveSolver)
	{
		return;
	}

	UOceanologyGlobalDisplacementHelper::SetNiagaraVariables(NiagaraWavesComponent,
	                                                         GerstnerWaveSolver->GlobalDisplacement);
	UOceanologyBaseOffsetHelper::SetNiagaraVariables(NiagaraWavesComponent, GerstnerWaveSolver->BaseOffset);
	UOceanologyWavesHelper::SetNiagaraVariablesWave_1(NiagaraWavesComponent, GerstnerWaveSolver->Wave_1);
	UOceanologyWavesHelper::SetNiagaraVariablesWave_2(NiagaraWavesComponent, GerstnerWaveSolver->Wave_2);
	UOceanologyWavesHelper::SetNiagaraVariablesWave_3(NiagaraWavesComponent, GerstnerWaveSolver->Wave_3);
	UOceanologyWavesHelper::SetNiagaraVariablesWave_4(NiagaraWavesComponent, GerstnerWaveSolver->Wave_4);
}

void AOceanologyWaterNiagaraWavesVolume::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void AOceanologyWaterNiagaraWavesVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Init();
}
