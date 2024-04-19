// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Actors/OceanologyLakeActor.h"
#include "NiagaraComponent.h"
#include "OceanologyRuntimeSettings.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/OceanAudioComponent.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/QuadTree/OceanologyWaterMeshComponent.h"
#include "Components/Wave/OceanologyWaveSolverComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Utils/OceanologyWaterUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "DataAsset/OceanologyLakePreset.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"

AOceanologyLake::AOceanologyLake()
{
	UnderwaterBoxVolumeComponent->SetRelativeLocation(FVector(0.0, 0.0, -550.0));
	UnderwaterBoxVolumeComponent->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	UnderwaterBoxVolumeComponent->CanCharacterStepUpOn = ECB_No;
	UnderwaterBoxVolumeComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	UnderwaterBoxVolumeComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	UnderwaterBoxVolumeComponent->InitBoxExtent(FVector(1000.0, 1000.0, 1400.0));
	UnderwaterBoxVolumeComponent->SetLineThickness(10.0);

	UnderwaterPostProcessComponent->Priority = 99;
	UnderwaterPostProcessComponent->BlendRadius = 0.0;
	UnderwaterPostProcessComponent->BlendWeight = 1.0;
	UnderwaterPostProcessComponent->bUnbound = false;
	// UnderwaterPostProcessComponent->bUseAttachParentBound = true;

	VolumetricFogMeshComponent->SetRelativeScale3D(FVector(20.0, 20.0, 30.0));
	VolumetricFogMeshComponent->SetRelativeLocation(FVector(0.0, 0.0, -500.0));

	OceanAudioUnderwaterComponent = CreateDefaultSubobject<UOceanAudioComponent>("OceanAudioUnderwater");
	OceanAudioUnderwaterComponent->Mode = EOceanAudioMode::Underwater;
	OceanAudioUnderwaterComponent->SetupAttachment(DefaultSceneRoot);

	OceanAudioWaveComponent = CreateDefaultSubobject<UOceanAudioComponent>("OceanAudioWave");
	OceanAudioWaveComponent->Mode = EOceanAudioMode::Wave;
	OceanAudioWaveComponent->SetupAttachment(DefaultSceneRoot);

	CausticsComponent = CreateDefaultSubobject<UDecalComponent>("Caustics");
	CausticsComponent->SetupAttachment(WaterMeshComponent);
	CausticsComponent->DecalSize = FVector(32.0, 32.0, 32.0);
	CausticsComponent->SetRelativeScale3D(FVector(30, 30, 50.0));
	CausticsComponent->SetRelativeLocation(FVector(0.0, 0.0, -500.0));

	bEnableAutoLODGeneration = false;
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

TSubclassOf<UOceanologyWaveSolverComponent> AOceanologyLake::GetWaveSolverClass()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeWaveSolverClass();
}

void AOceanologyLake::Init()
{
	Super::Init();

	if (!SetupWaveSolvers())
	{
		return;
	}

	if (Preset)
	{
#if WITH_EDITOR
		Preset->OnPropertyUpdated.RemoveAll(this);
		Preset->OnPropertyUpdated.AddUObject(this, &AOceanologyLake::PostEditChange);
#endif

		if (PresetMode == EOceanologyWaterPresetMode::UsePresetDirectly)
		{
			Preset->LoadPreset(this, PresetInclusionGroups);
		}
	}

	InitSurface();

	if (WaveSolver)
	{
		WaveSolver->InitWaves(this, false);
	}

	if (UnderwaterComponent)
	{
		UnderwaterComponent->Init();
	}

	InitCaustics();
	RegisterUnderwaterOverlapEvents();

	WaveSolver->UpdateWaves();

	InitAudio();
}

#if WITH_EDITOR
void AOceanologyLake::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyLake, WaveSolverClass))
	{
		OnWaveSolverPropertyChanged();
		Super::PostEditChangeProperty(PropertyChangedEvent);
		return;
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyLake, Preset))
	{
		OnPresetPropertyChanged(PresetMode, Preset, PresetInclusionGroups);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AOceanologyLake::InitCaustics()
{
	if (!DefaultSceneRoot)
	{
		return;
	}

	if (!EnableCausticsOnGround)
	{
		CausticsMID = nullptr;
		CausticsComponent->SetVisibility(false);
		CausticsComponent->Deactivate();
		return;
	}

	CausticsComponent->Activate(true);

	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();

	if (CausticsMaterial == nullptr)
	{
		CausticsMaterial = OceanologyRuntimeSettings->GetLakeCausticsMaterial();
	}

	CreateOrUpdateCausticsMID();

	UOceanologyCausticsHelper::SetMaterialParameters(CausticsMID, Caustics);
	UOceanologyGroundCausticsHelper::SetMaterialParameters(CausticsMID, GroundCaustics);
	UOceanologyWaterProjectionHelper::SetMaterialParameters(CausticsMID, WaterProjection);
	CausticsMID->SetVectorParameterValue(
		"LocationOffset",
		FLinearColor(0, 0, DefaultSceneRoot->GetComponentLocation().Z, 1.0)
	);
	CausticsComponent->SetMaterial(0, CausticsMID);
	CausticsComponent->SetVisibility(true);
}

void AOceanologyLake::InitAudio()
{
	OceanAudioWaveComponent->bOverrideAttenuation = true;
	OceanAudioWaveComponent->AttenuationOverrides.FalloffDistance = 1000 * ((GetActorScale3D().X * GetActorScale3D().Y)
		/ 20);
}

void AOceanologyLake::LoadPreset(UOceanologyLakePreset* InPreset)
{
	if (!InPreset)
	{
		return;
	}

	Preset = InPreset;
	Preset->LoadPreset(this, PresetInclusionGroups);
}

void AOceanologyLake::RegisterUnderwaterOverlapEvents()
{
	if (!UnderwaterBoxVolumeComponent)
	{
		return;
	}

	UnderwaterBoxVolumeComponent->OnComponentBeginOverlap.Clear();
	UnderwaterBoxVolumeComponent->OnComponentBeginOverlap.AddDynamic(this, &AOceanologyLake::OnUnderwater_BeginOverlap);

	UnderwaterBoxVolumeComponent->OnComponentEndOverlap.Clear();
	UnderwaterBoxVolumeComponent->OnComponentEndOverlap.AddDynamic(this, &AOceanologyLake::OnUnderwater_EndOverlap);
}

void AOceanologyLake::OnUnderwater_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (DebugEnabled)
		{
			UKismetSystemLibrary::PrintString(
				this,
				"Lake overlap - BEGIN.",
				true,
				true,
				FLinearColor::Green,
				1.0
			);
		}

		OceanAudioUnderwaterComponent->Activate(true);

		if (UnderwaterComponent->BubblesSettings.EnableBubbles == EnabledOnInteraction)
		{
			BubblesComponent->Activate();
			BubblesComponent->SetVisibility(true, false);
		}
	}
}

void AOceanologyLake::OnUnderwater_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (DebugEnabled)
		{
			UKismetSystemLibrary::PrintString(
				this,
				"Lake overlap - END.",
				true,
				true,
				FLinearColor::Green,
				1.0
			);
		}

		OceanAudioUnderwaterComponent->Deactivate();

		if (UnderwaterComponent->BubblesSettings.EnableBubbles == EnabledOnInteraction)
		{
			BubblesComponent->SetVisibility(false, false);
			BubblesComponent->Deactivate();
		}
	}
}

FOceanologyWaterQuadTreeSettings AOceanologyLake::GetQuadTreeSettings()
{
	return QuadTreeSettings;
}

UMaterialInterface* AOceanologyLake::GetDefaultMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeGerstnerWaveMaterial();
}

UMaterialInterface* AOceanologyLake::GetDefaultFarMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeGerstnerWaveFarMaterial();
}

UMaterialInterface* AOceanologyLake::GetUnderwaterMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeUnderwaterMaterial();
}

UMaterialInterface* AOceanologyLake::GetUnderwaterColorlessMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeUnderwaterColorlessMaterial();
}

UMaterialInterface* AOceanologyLake::GetVolumetricFogMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeVolumetricFogMaterial();
}

UMaterialInterface* AOceanologyLake::GetBubblesMaterial()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeBubblesMaterial();
}

UNiagaraSystem* AOceanologyLake::GetBubblesEffect()
{
	return GetDefault<UOceanologyRuntimeSettings>()->GetLakeBubblesEffect();
}

void AOceanologyLake::CreateOrUpdateCausticsMID()
{
	if (!GetWorld())
	{
		return;
	}

	CausticsMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		CausticsMID,
		TEXT("CausticsMID"),
		CausticsMaterial,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* AOceanologyLake::GetCausticsMID()
{
	CreateOrUpdateCausticsMID();

	return CausticsMID;
}

int32 AOceanologyLake::GetDefaultWaterVolumeOverlapPriority()
{
	return 2;
}

FName AOceanologyLake::GetGameTimeInSecondsVariableName()
{
	return "GameTimeInSeconds_Lake";
}

void AOceanologyLake::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOceanologyLake, GroundCaustics);
	DOREPLIFETIME(AOceanologyLake, WaterProjection);
}
