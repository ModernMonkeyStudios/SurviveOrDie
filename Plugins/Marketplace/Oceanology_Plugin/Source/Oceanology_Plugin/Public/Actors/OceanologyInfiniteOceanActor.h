// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "OceanologyRuntimeSettings.h"
#include "OceanologyWaterParentActor.h"
#include "Enums/OceanologyWaterPresetMode.h"
#include "GameFramework/Actor.h"
#include "Structs/OceanologyOceanQuadTreeSettings.h"
#include "Structs/OceanologyWetness.h"
#include "OceanologyInfiniteOceanActor.generated.h"

class UOceanologyWaveSolverComponent;
class UOceanologyOceanPreset;
class UWorld;
class UOceanAudioComponent;
class UOceanologyInfiniteComponent;
class UHierarchicalInstancedStaticMeshComponent;
class UStaticMeshComponent;
class UDecalComponent;
class UMaterialInterface;
class UMaterialParameterCollection;
class UMaterialInstanceDynamic;

UCLASS(Blueprintable)
class OCEANOLOGY_PLUGIN_API AOceanologyInfiniteOcean : public AOceanologyWaterParent
{
	GENERATED_BODY()

public:
	AOceanologyInfiniteOcean();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UOceanologyInfiniteComponent> InfiniteComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UDecalComponent> DLWetnessComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UOceanAudioComponent> OceanAudioUnderwaterComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = QuadTree)
	FOceanologyOceanQuadTreeSettings QuadTreeSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
	TObjectPtr<UOceanologyOceanPreset> Preset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
	EOceanologyWaterPresetMode PresetMode = EOceanologyWaterPresetMode::UsePresetDirectly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
	TArray<FName> PresetInclusionGroups = {
		"SurfaceScattering",
		"Caustics",
		"Refraction",
		"HorizonCorrection",
		"Flipbook",
		"Foam",
		"Folding",
		"Procedural",
		// "RVT", // RVT should be set per each landscape
		"Mask",
		"GGX",
		"UnderwaterMode",
		"Underwater",
		"VolumetricFog",
		"MaskUnderwater",
		"BubblesSettings",
		"WaterProjection",
		"GlobalDisplacement",
		"BaseOffset",
		"Wave_1",
		"Wave_2",
		"Wave_3",
		"Wave_4",
		"Summarize",
		"EnableWetness",
		"Wetness",
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness")
	bool EnableWetness = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness", Replicated)
	FOceanologyWetness Wetness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness")
	UMaterialInterface* ShorelineWetnessMaterial = nullptr;

	virtual TSubclassOf<UOceanologyWaveSolverComponent> GetWaveSolverClass() override;

	UFUNCTION(CallInEditor, BlueprintCallable, Category="Infinite",
		meta = (ToolTip = "Can be used to call a force follow on the actor when follow in editor is disabled."))
	void ForceFollow();

	virtual void Init() override;

	UFUNCTION(Category= "Ocean")
	void InitShorelineWetness();

	UFUNCTION(BlueprintCallable, Category= "Ocean")
	void LoadPreset(UOceanologyOceanPreset* InPreset);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual FOceanologyWaterQuadTreeSettings GetQuadTreeSettings() override;

	virtual UMaterialInterface* GetDefaultMaterial() override;
	virtual UMaterialInterface* GetDefaultFarMaterial() override;
	virtual UMaterialInterface* GetUnderwaterMaterial() override;
	virtual UMaterialInterface* GetUnderwaterColorlessMaterial() override;
	virtual UMaterialInterface* GetVolumetricFogMaterial() override;
	virtual UMaterialInterface* GetBubblesMaterial() override;
	virtual UNiagaraSystem* GetBubblesEffect() override;

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateShorelineWetnessMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	UMaterialInstanceDynamic* GetShorelineWetnessMID();
	
	virtual int32 GetDefaultWaterVolumeOverlapPriority() override;
	virtual FName GetGameTimeInSecondsVariableName() override;

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	UMaterialInstanceDynamic* ShorelineWetnessMID;
};
