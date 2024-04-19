// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "OceanologyWaterNiagaraWavesVolume.generated.h"

class AOceanologyWaterParent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class OCEANOLOGY_PLUGIN_API AOceanologyWaterNiagaraWavesVolume : public AVolume
{
	GENERATED_BODY()

public:
	AOceanologyWaterNiagaraWavesVolume();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UNiagaraComponent> NiagaraWavesComponent;

	/** The owner water of this water volume. Mandatory. Used to determine wave height. Physics & swimming logic's fundamental settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	TObjectPtr<AOceanologyWaterParent> OceanologyWater = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	TObjectPtr<UNiagaraSystem> NiagaraWaves = nullptr;

	UFUNCTION(CallInEditor, Category = "Default", BlueprintCallable)
	void ReloadSettings();

	UFUNCTION(CallInEditor, Category = "Default", BlueprintCallable)
	void DebugBounds();

	void Init();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
