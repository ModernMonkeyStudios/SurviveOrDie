// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Components/OceanAudioComponent.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyInfiniteOceanActor.h"
#include "Actors/OceanologyLakeActor.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UOceanAudioComponent::UOceanAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
	bTickInEditor = false;

	InitSound();
}

void UOceanAudioComponent::Init()
{
	if (!OceanologyWater)
	{
		if (GetOwner()->IsA(AOceanologyWaterParent::StaticClass()))
		{
			// Use the parent actor if WaterParent.
			OceanologyWater = Cast<AOceanologyWaterParent>(GetOwner());
		}
		else
		{
			// Use the DefaultOcean if not WaterParent.
			OceanologyWater = Cast<AOceanologyInfiniteOcean>(
				UGameplayStatics::GetActorOfClass(
					this,
					AOceanologyInfiniteOcean::StaticClass()
				)
			);
		}
	}

	if (!OceanologyWater)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"OceanologyWater is not configured! Ocean Audio component will NOT work!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);

		Deactivate();
		return;
	}

	InitSound();

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"OceanAudio activated for " + OceanologyWater->GetName() + "!",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanAudioComponent::InitSound()
{
	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();

	switch (Mode)
	{
	case EOceanAudioMode::Underwater:
		if (GetOwner()->IsA(AOceanologyInfiniteOcean::StaticClass()))
		{
			Sound = OceanologyRuntimeSettings->GetCue_Underwater_Ocean();
		}

		if (GetOwner()->IsA(AOceanologyLake::StaticClass()))
		{
			Sound = OceanologyRuntimeSettings->GetCue_Underwater_Lake();
		}
		bOverrideAttenuation = false;
		break;

	case EOceanAudioMode::Wave:
		if (GetOwner()->IsA(AOceanologyLake::StaticClass()))
		{
			Sound = OceanologyRuntimeSettings->GetCue_Wave_Lake();
		}
		else
		{
			Sound = OceanologyRuntimeSettings->GetCue_Wave_Ocean();
		}

		bOverrideAttenuation = true;
		break;

	default: ;
	}
}

void UOceanAudioComponent::PlaySoundFX()
{
	if (!GetOwner()->HasActorBegunPlay() || !OceanologyWater || Mode == EOceanAudioMode::None || !ActiveAudio)
	{
		Stop();
		return;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		Stop();
		return;
	}

	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const FVector WaveHeightAtCameraLocation = OceanologyWater->GetWaveHeightAtLocation(CameraLocation);

	bool ShouldPlay = false;

	switch (Mode)
	{
	case EOceanAudioMode::Underwater:
		if (WaveHeightAtCameraLocation.Z > CameraLocation.Z)
		{
			ShouldPlay = true;
		}
		else
		{
			ShouldPlay = false;
		}
		break;

	case EOceanAudioMode::Wave:
		if (WaveHeightAtCameraLocation.Z < CameraLocation.Z)
		{
			ShouldPlay = true;
		}
		else
		{
			ShouldPlay = false;
		}
		break;

	case EOceanAudioMode::None:
		break;

	default:
		break;
	}

	if (ShouldPlay)
	{
		if (!IsPlaying())
		{
			Play(0);
		}
	}
	else
	{
		Stop();
	}

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"OceanAudio Mode: " + UEnum::GetDisplayValueAsText(Mode).ToString()
			+ ", Actor: " + OceanologyWater->GetName()
			+ ", ShouldPlay: " + UKismetStringLibrary::Conv_BoolToString(ShouldPlay),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanAudioComponent::Activate(const bool bReset)
{
	Super::Activate(bReset);

	Init();

	ActiveAudio = true;

	PlaySoundFX();
}

void UOceanAudioComponent::Deactivate()
{
	Super::Deactivate();

	ActiveAudio = false;

	Stop();

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"OceanAudio deactivated for " + (OceanologyWater != nullptr ? OceanologyWater->GetName() : "unknown") + "!",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanAudioComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Deactivate();
}

void UOceanAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	Deactivate();
}


void UOceanAudioComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ActiveAudio)
	{
		return;
	}

	PlaySoundFX();
}
