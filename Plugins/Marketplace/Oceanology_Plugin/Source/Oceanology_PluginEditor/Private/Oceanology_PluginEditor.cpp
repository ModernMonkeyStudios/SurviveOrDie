// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Oceanology_PluginEditor.h"
#include "OceanologyWaterUIStyle.h"
#include "..\Public\Utils\OceanologyManagerDetailsCustomization.h"
#include "Utils/OceanologyWaterParentDetailsCustomization.h"
#include "Utils/OceanologyWaterVolumeDetailsCustomization.h"

#define LOCTEXT_NAMESPACE "OceanologyEditor"

IMPLEMENT_MODULE(FOceanology_PluginEditor, OceanologyEditor);

#undef LOCTEXT_NAMESPACE

void FOceanology_PluginEditor::StartupModule()
{
	IOceanology_PluginEditorInterface::StartupModule();

	FOceanologyWaterUIStyle::Initialize();

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		"OceanologyManager",
		FOnGetDetailCustomizationInstance::CreateStatic(&FOceanologyManagerDetailsCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomClassLayout(
		"OceanologyWaterVolume",
		FOnGetDetailCustomizationInstance::CreateStatic(&FOceanologyWaterVolumeDetailsCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomClassLayout(
		"OceanologyWaterParent",
		FOnGetDetailCustomizationInstance::CreateStatic(&FOceanologyWaterParentDetailsCustomization::MakeInstance)
	);

	GEngine->OnLevelActorAdded().AddRaw(this, &FOceanology_PluginEditor::OnLevelActorAddedToWorld);
}

void FOceanology_PluginEditor::ShutdownModule()
{
	FOceanologyWaterUIStyle::Shutdown();
}

void FOceanology_PluginEditor::OnLevelActorAddedToWorld(AActor* Actor)
{
	// there might be room for future actors here
}
