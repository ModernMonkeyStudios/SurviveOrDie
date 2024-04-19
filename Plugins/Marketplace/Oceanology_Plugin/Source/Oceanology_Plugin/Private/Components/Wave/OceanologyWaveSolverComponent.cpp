// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Components/Wave/OceanologyWaveSolverComponent.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Components/QuadTree/OceanologyWaterMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Materials/MaterialParameterCollection.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "Structs/OceanologyWaterPresetResult.h"
#include "Utils/OceanologyMessageUtils.h"

UOceanologyWaveSolverComponent::UOceanologyWaveSolverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = false;
}

void UOceanologyWaveSolverComponent::InitWaves_Implementation(
	AOceanologyWaterParent* InOwner,
	const bool InUsingMPC_Waves
)
{
	Owner = InOwner;
	UsingMPC_Waves = InUsingMPC_Waves;

	if (!Owner)
	{
		return;
	}

	if (AActor* Actor = UGameplayStatics::GetActorOfClass(this, GetOwner()->GetClass());
		Actor && Actor->IsA(GetOwner()->GetClass()))
	{
		FirstWaterInstance = Cast<AOceanologyWaterParent>(Actor);
	}

	if (!FirstWaterInstance)
	{
		return;
	}

	if (!FirstWaterInstance->GetWaveSolver())
	{
		return;
	}

	if (Owner->GetGameTimeInSecondsVariableName() == "")
	{
		return;
	}

	if (!MPC_Waves)
	{
		MPC_Waves = GetDefault<UOceanologyRuntimeSettings>()->GetMPC_Waves();
	}

	SetComponentTickInterval(WaveSolverUpdateInterval);
}

void UOceanologyWaveSolverComponent::UpdateWaves_Implementation()
{
	if (!Owner || !Owner->WaterMeshComponent)
	{
		return;
	}

	Owner->WaterMeshComponent->SetMaxWaveHeight(GetMaxWaveHeight());
}

void UOceanologyWaveSolverComponent::UpdateWavesByPresetResult_Implementation(
	AOceanologyWaterParent* Water,
	const FOceanologyWaterPresetResult& WaterPresetResult
)
{
	Water->SurfaceScattering = WaterPresetResult.SurfaceScattering;
	Water->Caustics = WaterPresetResult.Caustics;
	Water->Refraction = WaterPresetResult.Refraction;
	Water->HorizonCorrection = WaterPresetResult.HorizonCorrection;
	Water->Flipbook = WaterPresetResult.Flipbook;
	Water->Foam = WaterPresetResult.Foam;
	Water->Folding = WaterPresetResult.Folding;
	Water->Procedural = WaterPresetResult.Procedural;
	Water->RVT = WaterPresetResult.RVT;
	Water->Mask = WaterPresetResult.Mask;
	// Water->UnderwaterComponent->UnderwaterMode=WaterPresetResult.UnderwaterMode;
	Water->UnderwaterComponent->Underwater = WaterPresetResult.Underwater;
	Water->UnderwaterComponent->VolumetricFog = WaterPresetResult.VolumetricFog;
	Water->UnderwaterComponent->MaskUnderwater = WaterPresetResult.MaskUnderwater;
	// Water->UnderwaterComponent->BubblesSettings = WaterPresetResult.BubblesSettings;
	Water->UnderwaterComponent->WaterProjection = WaterPresetResult.WaterProjection;
}

FVector UOceanologyWaveSolverComponent::GetWaveHeightAtLocation_Implementation(const FVector& Location)
{
	return FVector::ZeroVector;
}

double UOceanologyWaveSolverComponent::GetMaxWaveHeight_Implementation()
{
	return 0.0;
}

double UOceanologyWaveSolverComponent::GetGlobalWaveDirection_Implementation()
{
	return 0;
}

double UOceanologyWaveSolverComponent::GetMinWaveHeight_Implementation()
{
	return -GetMaxWaveHeight();
}

bool UOceanologyWaveSolverComponent::IsUsingMPC_Waves() const
{
	return UsingMPC_Waves;
}

void UOceanologyWaveSolverComponent::StartWaveSimulation()
{
	if (!FirstWaterInstance)
	{
		return;
	}

	FirstWaterInstance->GetWaveSolver()->SimulateWaves = true;

	UOceanologyMessageUtils::CreateMessage(
		this,
		"Wave Simulation started of all " + Owner->GetClass()->GetName() + "(s).",
		true
	);
}

void UOceanologyWaveSolverComponent::StopWaveSimulation()
{
	if (!FirstWaterInstance)
	{
		return;
	}

	FirstWaterInstance->GetWaveSolver()->SimulateWaves = false;

	UOceanologyMessageUtils::CreateMessage(
		this,
		"Wave Simulation stopped of all " + Owner->GetClass()->GetName() + "(s).",
		true
	);
}

void UOceanologyWaveSolverComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive()
		|| !MPC_Waves
		|| !SimulateWaves
		|| !Owner
		|| !FirstWaterInstance)
	{
		return;
	}

	SetGameTimeInSeconds();
}

void UOceanologyWaveSolverComponent::SetGameTimeInSeconds()
{
	if (Owner != FirstWaterInstance)
	{
		const UOceanologyWaveSolverComponent* FirstWaveSolver = FirstWaterInstance->GetWaveSolver();
		GameTimeInSeconds = FirstWaveSolver->GameTimeInSeconds;
		SimulateWaves = FirstWaveSolver->SimulateWaves;
		return;
	}

	if (const AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
		GameState && WaveTimeMode == EWaveTimeMode::ServerTimeForReplication)
	{
		GameTimeInSeconds = GameState->GetServerWorldTimeSeconds();
	}
	else
	{
		GameTimeInSeconds = UKismetSystemLibrary::GetGameTimeInSeconds(this);
	}

	UMaterialParameterCollectionInstance* MPCI = GetWorld()->GetParameterCollectionInstance(
		MPC_Waves
	);

	MPCI->SetScalarParameterValue(Owner->GetGameTimeInSecondsVariableName(), GameTimeInSeconds);
}

void UOceanologyWaveSolverComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOceanologyWaveSolverComponent, Owner);
}
