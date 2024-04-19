// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "DataAsset/OceanologyLakePresetFactory.h"
#include "DataAsset/OceanologyLakePreset.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "OceanologyRuntimeSettings.h"

#define LOCTEXT_NAMESPACE "OceanologyEditor"

UOceanologyLakePresetFactory::UOceanologyLakePresetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UOceanologyLakePreset::StaticClass();
}

UObject* UOceanologyLakePresetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                                        EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UOceanologyLakePreset* OceanologyPreset = NewObject<UOceanologyLakePreset>(InParent, Class, Name, Flags);
	OceanologyPreset->BubblesSettings.EnableBubbles = EnabledOnInteraction;
	OceanologyPreset->BubblesSettings.BubblesEffect = GetDefault<UOceanologyRuntimeSettings>()->GetLakeBubblesEffect();
	OceanologyPreset->BubblesSettings.BubblesMaterial = GetDefault<UOceanologyRuntimeSettings>()->
		GetLakeBubblesMaterial();
	return OceanologyPreset;
}

FText UOceanologyLakePresetFactory::GetDisplayName() const
{
	return LOCTEXT("OceanologyLakePresetText", "Oceanology Lake Preset");
}

FString UOceanologyLakePresetFactory::GetDefaultNewAssetName() const
{
	return FString(TEXT("NewLakePreset"));
}
