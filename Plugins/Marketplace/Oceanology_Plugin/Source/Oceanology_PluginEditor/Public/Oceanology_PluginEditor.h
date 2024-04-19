// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Engine/EngineTypes.h"
#include "Toolkits/AssetEditorToolkit.h"

class IOceanology_PluginEditorInterface : public IModuleInterface
{
public:
};

class FOceanology_PluginEditor : public IOceanology_PluginEditorInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
	void OnLevelActorAddedToWorld(AActor* Actor);
};
