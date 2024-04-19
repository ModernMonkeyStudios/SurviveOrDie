// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Actors/OceanologyInfiniteOceanActor.h"
#include "NiagaraComponent.h"
#include "OceanologyRuntimeSettings.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/OceanologyInfiniteComponent.h"
#include "Components/OceanAudioComponent.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Utils/OceanologyWaterUtils.h"
#include "Components/Wave/OceanologyWaveSolverComponent.h"
#include "DataAsset/OceanologyOceanPreset.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Net/UnrealNetwork.h"
#include "Structs/OceanologyWetness.h"
#include "Engine/World.h"
#include "Utils/OceanologyMessageUtils.h"

AOceanologyInfiniteOcean::AOceanologyInfiniteOcean()
{
	PrimaryActorTick.bCanEverTick = true;

	InfiniteComponent = CreateDefaultSubobject<UOceanologyInfiniteComponent>(TEXT("Infinite"));

	UnderwaterBoxVolumeComponent->SetRelativeLocation(FVector(0.0, 0.0, -3975000.0));
	UnderwaterBoxVolumeComponent->CanCharacterStepUpOn = ECB_No;
	UnderwaterBoxVolumeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UnderwaterBoxVolumeComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	UnderwaterBoxVolumeComponent->InitBoxExtent(FVector(80000.0, 80000.0, 4000000.0));
	UnderwaterBoxVolumeComponent->SetLineThickness(100.0);

	UnderwaterPostProcessComponent->Priority = 98;
	UnderwaterPostProcessComponent->BlendRadius = 0.0;
	UnderwaterPostProcessComponent->BlendWeight = 1.0;
	UnderwaterPostProcessComponent->bUnbound = false;

	VolumetricFogMeshComponent->SetRelativeScale3D(FVector(500.0, 500.0, 5000.0));
	VolumetricFogMeshComponent->SetRelativeLocation(FVector(0.0, 0.0, -247500.0));

	DLWetnessComponent = CreateDefaultSubobject<UDecalComponent>("DLWetness");
	DLWetnessComponent->SetMobility(EComponentMobility::Movable);
	DLWetnessComponent->SetupAttachment(DefaultSceneRoot);
	DLWetnessComponent->DecalSize = FVector(100000.0, 100000.0, 100000.0);

	OceanAudioUnderwaterComponent = CreateDefaultSubobject<UOceanAudioComponent>("OceanAudioUnderwater");
	OceanAudioUnderwaterComponent->Mode = EOceanAudioMode::Underwater;
	OceanAudioUnderwaterComponent->SetupAttachment(DefaultSceneRoot);

	bReplicates = true;

	bEnableAutoLODGeneration = false;
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

TSubclassOf<UOceanologyWaveSolverComponent> AOceanologyInfiniteOcean::GetWaveSolverClass()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanWaveSolverClass();
}

void AOceanologyInfiniteOcean::ForceFollow()
{
	InfiniteComponent->ForceFollow();
}

void AOceanologyInfiniteOcean::Init()
{
	Super::Init();

	// The Ocean does NOT support on interact enable logic, as the Ocean has no collision.
	// Reverting to default "enabled" logic.
	if (UnderwaterComponent->BubblesSettings.EnableBubbles == EnabledOnInteraction)
	{
		UnderwaterComponent->BubblesSettings.EnableBubbles = Enabled;
	}

	// The Ocean does NOT need this attachment. This is to FIX actor relocation issues.
	BubblesComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	// The Ocean must not initialize if the wave solver failed.
	if (!SetupWaveSolvers())
	{
		return;
	}

	if (Preset)
	{
#if WITH_EDITOR
		Preset->OnPropertyUpdated.RemoveAll(this);
		Preset->OnPropertyUpdated.AddUObject(this, &AOceanologyInfiniteOcean::PostEditChange);
#endif

		if (PresetMode == EOceanologyWaterPresetMode::UsePresetDirectly)
		{
			Preset->LoadPreset(this, PresetInclusionGroups);
		}
	}

	InfiniteComponent->Init(this, GetWorld());

	InitSurface();

	if (WaveSolver)
	{
		WaveSolver->InitWaves(this, true);
	}

	if (UnderwaterComponent)
	{
		UnderwaterComponent->Init();
	}

	InitShorelineWetness();

	WaveSolver->UpdateWaves();

	if (HasActorBegunPlay())
	{
		OceanAudioUnderwaterComponent->Activate(true);
	}
}

void AOceanologyInfiniteOcean::InitShorelineWetness()
{
	if (!EnableWetness)
	{
		ShorelineWetnessMID = nullptr;
		DLWetnessComponent->SetVisibility(false, false);
		DLWetnessComponent->Deactivate();
		return;
	}

	DLWetnessComponent->Activate(true);

	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();

	if (ShorelineWetnessMaterial == nullptr)
	{
		ShorelineWetnessMaterial = OceanologyRuntimeSettings->GetOceanShorelineWetnessMaterial();
	}

	ShorelineWetnessMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		ShorelineWetnessMID,
		TEXT("ShorelineWetnessMID"),
		ShorelineWetnessMaterial,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);

	UOceanologyWetnessHelper::SetMaterialParameters(ShorelineWetnessMID, Wetness);
	UOceanologyRVTHelper::SetMaterialParameters(ShorelineWetnessMID, RVT);

	DLWetnessComponent->SetMaterial(0, ShorelineWetnessMID);
	DLWetnessComponent->SetVisibility(true, false);
}

void AOceanologyInfiniteOcean::LoadPreset(UOceanologyOceanPreset* InPreset)
{
	if (InPreset == nullptr)
	{
		return;
	}

	Preset = InPreset;
	Preset->LoadPreset(this, PresetInclusionGroups);
}

#if WITH_EDITOR
void AOceanologyInfiniteOcean::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyInfiniteOcean, WaveSolverClass))
	{
		OnWaveSolverPropertyChanged();
		Super::PostEditChangeProperty(PropertyChangedEvent);
		return;
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyInfiniteOcean, Preset))
	{
		OnPresetPropertyChanged(PresetMode, Preset, PresetInclusionGroups);
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FOceanologyRVT, WaterZMax)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FOceanologyRVT, WaterZMin)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FOceanologyMask, LandscapeVisibilityMask))
	{
		WaveSolver->UpdateWaves();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FOceanologyWaterQuadTreeSettings AOceanologyInfiniteOcean::GetQuadTreeSettings()
{
	return QuadTreeSettings;
}

UMaterialInterface* AOceanologyInfiniteOcean::GetDefaultMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanGerstnerWaveMaterial();
}

UMaterialInterface* AOceanologyInfiniteOcean::GetDefaultFarMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanGerstnerWaveFarMaterial();
}

UMaterialInterface* AOceanologyInfiniteOcean::GetUnderwaterMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanUnderwaterMaterial();
}

UMaterialInterface* AOceanologyInfiniteOcean::GetUnderwaterColorlessMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanUnderwaterColorlessMaterial();
}

UMaterialInterface* AOceanologyInfiniteOcean::GetVolumetricFogMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanVolumetricFogMaterial();
}

UMaterialInterface* AOceanologyInfiniteOcean::GetBubblesMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanBubblesMaterial();
}

UNiagaraSystem* AOceanologyInfiniteOcean::GetBubblesEffect()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetOceanBubblesEffect();
}

void AOceanologyInfiniteOcean::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AOceanologyInfiniteOcean::CreateOrUpdateShorelineWetnessMID()
{
	if (!GetWorld())
	{
		return;
	}

	ShorelineWetnessMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		ShorelineWetnessMID,
		TEXT("ShorelineWetnessMID"),
		ShorelineWetnessMaterial,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* AOceanologyInfiniteOcean::GetShorelineWetnessMID()
{
	CreateOrUpdateShorelineWetnessMID();

	return ShorelineWetnessMID;
}

int32 AOceanologyInfiniteOcean::GetDefaultWaterVolumeOverlapPriority()
{
	return 1;
}

FName AOceanologyInfiniteOcean::GetGameTimeInSecondsVariableName()
{
	return "GameTimeInSeconds_Ocean";
}

void AOceanologyInfiniteOcean::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOceanologyInfiniteOcean, Wetness);
}
