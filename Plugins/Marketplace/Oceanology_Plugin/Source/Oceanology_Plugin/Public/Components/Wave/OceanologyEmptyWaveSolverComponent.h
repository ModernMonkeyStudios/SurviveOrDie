// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "OceanologyWaveSolverComponent.h"
#include "OceanologyEmptyWaveSolverComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanologyEmptyWaveSolverComponent : public UOceanologyWaveSolverComponent
{
	GENERATED_BODY()

	UOceanologyEmptyWaveSolverComponent();
};
