// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Components/OceanologyUnderwaterComponent.h"
#include "NiagaraComponent.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/PostProcessComponent.h"
#include "Utils/OceanologyWaterUtils.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Structs/OceanologyBubbles.h"
#include "Structs/OceanologyRVT.h"
#include "Utils/OceanologyMessageUtils.h"

UOceanologyUnderwaterComponent::UOceanologyUnderwaterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;
}


void UOceanologyUnderwaterComponent::Init()
{
	if (!GetOwner()->IsA(AOceanologyWaterParent::StaticClass()))
	{
		return;
	}

	OceanologyWater = Cast<AOceanologyWaterParent>(GetOwner());

	if (!OceanologyWater
		|| !OceanologyWater->DefaultSceneRoot
		|| !OceanologyWater->UnderwaterBoxVolumeComponent
		|| !OceanologyWater->UnderwaterPostProcessComponent)
	{
		return;
	}

	OceanologyWater->UnderwaterPostProcessComponent->Settings.WeightedBlendables.Array.Empty();

	UnderwaterMID = nullptr;
	VolumetricFogMID = nullptr;
	BubblesMID = nullptr;

	UnderwaterMaterial = nullptr;
	VolumetricFogMaterial = nullptr;

	OceanologyWater->UnderwaterPostProcessComponent->bEnabled = false;
	OceanologyWater->VolumetricFogMeshComponent->SetVisibility(false, false);

	bool UnderwaterEnabled = false;
	bool VolumetricFogMeshVisible = false;

	switch (UnderwaterMode)
	{
	case EOceanologyUnderwaterMode::None:
		return;

	case EOceanologyUnderwaterMode::Underwater:
		if (!UserOverrideUnderwaterMaterial || UnderwaterMaterial == nullptr)
		{
			UnderwaterMaterial = OceanologyWater->GetUnderwaterMaterial();
		}

		VolumetricFogMaterial = nullptr;
		UnderwaterEnabled = true;
		VolumetricFogMeshVisible = false;
		break;

	case EOceanologyUnderwaterMode::VolumetricFog:
		if (!UserOverrideUnderwaterMaterial || UnderwaterMaterial == nullptr)
		{
			UnderwaterMaterial = OceanologyWater->GetUnderwaterColorlessMaterial();
		}

		if (!UserOverrideUnderwaterMaterial || VolumetricFogMaterial == nullptr)
		{
			VolumetricFogMaterial = OceanologyWater->GetVolumetricFogMaterial();
		}
		UnderwaterEnabled = true;
		VolumetricFogMeshVisible = true;
		break;

	default: ;
	}

	if (!UnderwaterMaterial && UnderwaterMode != EOceanologyUnderwaterMode::None)
	{
		UOceanologyMessageUtils::CreateMessage(this, "Underwater Material is null! Cannot be initialized!", false);
		return;
	}

	CreateOrUpdateUnderwaterMID();

	UOceanologyUnderwaterHelper::SetMaterialParameters(UnderwaterMID, Underwater);
	UOceanologyRVTHelper::SetMaterialParameters(UnderwaterMID, OceanologyWater->RVT);
	UOceanologyMaskHelper::SetMaterialParameters(UnderwaterMID, OceanologyWater->Mask);
	UOceanologyMaskUnderwaterHelper::SetMaterialParameters(UnderwaterMID, MaskUnderwater);
	UOceanologyWaterProjectionHelper::SetMaterialParameters(UnderwaterMID, WaterProjection);
	UOceanologyFlipbookHelper::SetMaterialParameters(UnderwaterMID, OceanologyWater->Flipbook);
	UOceanologyActorHeightHelper::SetMaterialParameters(UnderwaterMID, OceanologyWater->ActorHeight);
	UnderwaterMID->SetVectorParameterValue(
		"LocationOffset",
		FLinearColor(0, 0, OceanologyWater->DefaultSceneRoot->GetComponentLocation().Z, 1.0)
	);

	OceanologyWater->UnderwaterPostProcessComponent->AddOrUpdateBlendable(UnderwaterMID, 1.0);

	if (VolumetricFogMaterial)
	{
		CreateOrUpdateVolumetricFogMID();

		OceanologyWater->VolumetricFogMeshComponent->SetMaterial(0, VolumetricFogMID);

		UOceanologyVolumetricFogHelper::SetMaterialParameters(VolumetricFogMID, VolumetricFog);
		UOceanologyRVTHelper::SetMaterialParameters(VolumetricFogMID, OceanologyWater->RVT);
		UOceanologyMaskHelper::SetMaterialParameters(VolumetricFogMID, OceanologyWater->Mask);
		UOceanologyMaskUnderwaterHelper::SetMaterialParameters(VolumetricFogMID, MaskUnderwater);
		UOceanologyWaterProjectionHelper::SetMaterialParameters(VolumetricFogMID, WaterProjection);
		UOceanologyFlipbookHelper::SetMaterialParameters(VolumetricFogMID, OceanologyWater->Flipbook);
		UOceanologyActorHeightHelper::SetMaterialParameters(VolumetricFogMID, OceanologyWater->ActorHeight);
	}

	OceanologyWater->UnderwaterPostProcessComponent->bEnabled = UnderwaterEnabled;
	OceanologyWater->VolumetricFogMeshComponent->SetVisibility(VolumetricFogMeshVisible, false);

	InitBubbles();
}

void UOceanologyUnderwaterComponent::InitBubbles()
{
	if (OceanologyWater->BubblesComponent)
	{
		OceanologyWater->BubblesComponent->DestroyInstance();
	}

	switch (BubblesSettings.EnableBubbles)
	{
	case Enabled:
		OceanologyWater->BubblesComponent->Activate();
		OceanologyWater->BubblesComponent->SetVisibility(true, false);
		break;

	case EnabledOnInteraction:
		OceanologyWater->BubblesComponent->SetVisibility(false, false);
		OceanologyWater->BubblesComponent->Deactivate();
		break;

	case Disabled:
		OceanologyWater->BubblesComponent->SetVisibility(false, false);
		OceanologyWater->BubblesComponent->Deactivate();
		return;

	default:
		ensureMsgf(false, TEXT("Unsupported bubble activation mode!"));
	}

	if (BubblesSettings.BubblesEffect == nullptr)
	{
		BubblesSettings.BubblesEffect = OceanologyWater->GetBubblesEffect();
	}

	if (BubblesSettings.BubblesMaterial == nullptr)
	{
		BubblesSettings.BubblesMaterial = OceanologyWater->GetBubblesMaterial();
	}

	CreateOrUpdateBubblesMID();

	OceanologyWater->BubblesComponent->SetAsset(BubblesSettings.BubblesEffect.LoadSynchronous());
	OceanologyWater->BubblesComponent->SetVariableMaterial("DynamicMaterial", BubblesMID);
	OceanologyWater->BubblesComponent->SetVariableInt("BubbleSpawnCount", BubblesSettings.BubbleSpawnCount);
	OceanologyWater->BubblesComponent->SetVariableFloat("BubbleSpeed", BubblesSettings.BubbleSpeed);

	UOceanologyBubblesHelper::SetMaterialParameters(BubblesMID, BubblesSettings.Bubbles);
	UOceanologyRVTHelper::SetMaterialParameters(BubblesMID, OceanologyWater->RVT);
	UOceanologyActorHeightHelper::SetMaterialParameters(BubblesMID, OceanologyWater->ActorHeight);
	BubblesMID->SetVectorParameterValue(
		"LocationOffset",
		FLinearColor(OceanologyWater->DefaultSceneRoot->GetComponentLocation())
	);
}

void UOceanologyUnderwaterComponent::CreateOrUpdateUnderwaterMID()
{
	if (!GetWorld())
	{
		return;
	}

	UnderwaterMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		UnderwaterMID,
		TEXT("UnderwaterMID"),
		UnderwaterMaterial,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* UOceanologyUnderwaterComponent::GetUnderwaterMID()
{
	CreateOrUpdateUnderwaterMID();

	return UnderwaterMID;
}

void UOceanologyUnderwaterComponent::CreateOrUpdateVolumetricFogMID()
{
	if (!GetWorld())
	{
		return;
	}

	VolumetricFogMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		VolumetricFogMID,
		TEXT("VolumetricFogMID"),
		VolumetricFogMaterial,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* UOceanologyUnderwaterComponent::GetVolumetricFogMID()
{
	CreateOrUpdateVolumetricFogMID();

	return VolumetricFogMID;
}

void UOceanologyUnderwaterComponent::CreateOrUpdateBubblesMID()
{
	if (!GetWorld())
	{
		return;
	}

	BubblesMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		BubblesMID,
		TEXT("BubblesMID"),
		BubblesSettings.BubblesMaterial.LoadSynchronous(),
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* UOceanologyUnderwaterComponent::GetBubblesMID()
{
	CreateOrUpdateBubblesMID();

	return BubblesMID;
}

void UOceanologyUnderwaterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOceanologyUnderwaterComponent, OceanologyWater);

	DOREPLIFETIME(UOceanologyUnderwaterComponent, UnderwaterMode);
	DOREPLIFETIME(UOceanologyUnderwaterComponent, Underwater);
	DOREPLIFETIME(UOceanologyUnderwaterComponent, VolumetricFog);
	DOREPLIFETIME(UOceanologyUnderwaterComponent, MaskUnderwater);
	DOREPLIFETIME(UOceanologyUnderwaterComponent, WaterProjection);
	DOREPLIFETIME(UOceanologyUnderwaterComponent, BubblesSettings);
}
