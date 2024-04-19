// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "DataAsset/OceanologyOceanPresetFactory.h"
#include "DataAsset/OceanologyOceanPreset.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "OceanologyRuntimeSettings.h"

#define LOCTEXT_NAMESPACE "OceanologyEditor"

UOceanologyOceanPresetFactory::UOceanologyOceanPresetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UOceanologyOceanPreset::StaticClass();
}

UObject* UOceanologyOceanPresetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                                         EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UOceanologyOceanPreset* OceanologyPreset = NewObject<UOceanologyOceanPreset>(InParent, Class, Name, Flags);
	OceanologyPreset->BubblesSettings.BubblesEffect = GetDefault<UOceanologyRuntimeSettings>()->GetOceanBubblesEffect();
	OceanologyPreset->BubblesSettings.BubblesMaterial = GetDefault<UOceanologyRuntimeSettings>()->
		GetOceanBubblesMaterial();
	return OceanologyPreset;
}

FText UOceanologyOceanPresetFactory::GetDisplayName() const
{
	return LOCTEXT("OceanologyOceanPresetText", "Oceanology Ocean Preset");
}

FString UOceanologyOceanPresetFactory::GetDefaultNewAssetName() const
{
	return FString(TEXT("NewOceanPreset"));
}
