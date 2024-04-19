// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Oceanology_Plugin.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FOceanology_PluginModule"

void FOceanology_PluginModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Oceanology_Plugin"))->GetBaseDir(),
	                                          TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/Oceanology_Plugin"), PluginShaderDir);
}

void FOceanology_PluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOceanology_PluginModule, Oceanology_Plugin)
