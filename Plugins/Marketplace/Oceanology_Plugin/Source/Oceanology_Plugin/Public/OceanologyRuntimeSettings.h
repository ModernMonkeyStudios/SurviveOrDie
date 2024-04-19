// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/EngineTypes.h"
#include "OceanologyRuntimeSettings.generated.h"

class UOceanologyWaveSolverComponent;
class UVirtualTextureBuilder;
class UMaterialInterface;
class UMaterialParameterCollection;
class UCanvasRenderTarget2D;
class UTextureRenderTarget2D;
class UNiagaraSystem;
class URuntimeVirtualTexture;
class USoundBase;

/**
 * Implements the runtime settings for the Water plugin.
 */
UCLASS(config = Engine, defaultconfig, meta=(DisplayName="Oceanology"))
class OCEANOLOGY_PLUGIN_API UOceanologyRuntimeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, config, Category = "Waves")
	TSubclassOf<UOceanologyWaveSolverComponent> LakeWaveSolverClass;

	UPROPERTY(EditAnywhere, config, Category = "Waves")
	TSubclassOf<UOceanologyWaveSolverComponent> OceanWaveSolverClass;

	UPROPERTY(EditAnywhere, config, Category = "Mesh|Lake")
	TSoftObjectPtr<UStaticMesh> LakeMesh;

	UPROPERTY(EditAnywhere, config, Category = "Mesh")
	TSoftObjectPtr<UStaticMesh> VolumetricFogMesh;

	UPROPERTY(EditAnywhere, config, Category = "Material|Lake")
	TSoftObjectPtr<UMaterialInterface> LakeBubblesMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Lake")
	TSoftObjectPtr<UMaterialInterface> LakeGerstnerWaveMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|OLake")
	TSoftObjectPtr<UMaterialInterface> LakeGerstnerWaveFarMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Lake")
	TSoftObjectPtr<UMaterialInterface> LakeUnderwaterMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Lake")
	TSoftObjectPtr<UMaterialInterface> LakeUnderwaterColorlessMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Lake")
	TSoftObjectPtr<UMaterialInterface> LakeVolumetricFogMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Lake")
	TSoftObjectPtr<UMaterialInterface> LakeCausticsMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanBubblesMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanGerstnerWaveMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanGerstnerWaveFarMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanUnderwaterMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanUnderwaterColorlessMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanVolumetricFogMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Material|Ocean")
	TSoftObjectPtr<UMaterialInterface> OceanShorelineWetnessMaterial;

	UPROPERTY(EditAnywhere, config, Category = "Gerstner Waves")
	TSoftObjectPtr<UMaterialParameterCollection> MPC_Waves;

	UPROPERTY(EditAnywhere, config, Category = "Wave Sounds")
	TSoftObjectPtr<USoundBase> Cue_Wave_Ocean;

	UPROPERTY(EditAnywhere, config, Category = "Wave Sounds")
	TSoftObjectPtr<USoundBase> Cue_Underwater_Ocean;

	UPROPERTY(EditAnywhere, config, Category = "Wave Sounds")
	TSoftObjectPtr<USoundBase> Cue_Wave_Lake;

	UPROPERTY(EditAnywhere, config, Category = "Wave Sounds")
	TSoftObjectPtr<USoundBase> Cue_Underwater_Lake;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap")
	TSoftObjectPtr<URuntimeVirtualTexture> RVT_Heightmap;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap")
	TSoftObjectPtr<UVirtualTextureBuilder> RVT_Heightmap_Builder;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap|Material")
	TSoftObjectPtr<UMaterialInterface> RT_SDF_CopyUVs;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap|Material")
	TSoftObjectPtr<UMaterialInterface> RT_SDF_JumpFlood;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap|Material")
	TSoftObjectPtr<UMaterialInterface> RT_SDF_ComputeSDF;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap|Material")
	TSoftObjectPtr<UMaterialInterface> RT_SDF_Blur;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|Landscape Heightmap|Material")
	TSoftObjectPtr<UMaterialInterface> RT_SDF_Compose;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|RVT Water Bodies")
	TSoftObjectPtr<URuntimeVirtualTexture> RVT_WaterBodies;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|RVT Water Bodies")
	TSoftObjectPtr<UVirtualTextureBuilder> RVT_WaterBodies_Builder;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|RVT Water Bodies")
	TSoftObjectPtr<UMaterialInterface> RVT_WaterBodies_CopyMaterial;

	UPROPERTY(EditAnywhere, config, Category = "DO NOT EDIT - Internal Factory Default|RVT Water Bodies")
	TSoftObjectPtr<UTextureRenderTarget2D> RT_WaterBodies;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Lake|Bubbles")
	TSoftObjectPtr<UNiagaraSystem> LakeBubblesEffect;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Ocean|Bubbles")
	TSoftObjectPtr<UNiagaraSystem> OceanBubblesEffect;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Swimming|Bubbles")
	TSoftObjectPtr<UNiagaraSystem> HeadBubblesEffectNiagara;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Swimming|Bubbles")
	TSoftObjectPtr<UNiagaraSystem> RightFootBubblesEffectNiagara;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Swimming|Bubbles")
	TSoftObjectPtr<UNiagaraSystem> LeftFootBubblesEffectNiagara;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Swimming|Bubbles")
	TSoftObjectPtr<UNiagaraSystem> SwimFastTrailEffectNiagara;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Swimming|Sound")
	TSoftObjectPtr<USoundBase> SwimFastTrailSound;

	UPROPERTY(EditAnywhere, Config, Category="Effects|Wave")
	TSoftObjectPtr<UNiagaraSystem> NiagaraGerstnerWavesEffect;

public:
	UOceanologyRuntimeSettings();

	virtual FName GetCategoryName() const override;

	TSubclassOf<UOceanologyWaveSolverComponent> GetLakeWaveSolverClass() const;
	TSubclassOf<UOceanologyWaveSolverComponent> GetOceanWaveSolverClass() const;

	UStaticMesh* GetLakeMesh() const;

	UStaticMesh* GetVolumetricFogMesh() const;

	UMaterialInterface* GetLakeBubblesMaterial() const;
	UMaterialInterface* GetLakeGerstnerWaveMaterial() const;
	UMaterialInterface* GetLakeGerstnerWaveFarMaterial() const;
	UMaterialInterface* GetLakeUnderwaterMaterial() const;
	UMaterialInterface* GetLakeUnderwaterColorlessMaterial() const;
	UMaterialInterface* GetLakeVolumetricFogMaterial() const;
	UMaterialInterface* GetLakeCausticsMaterial() const;

	UMaterialInterface* GetOceanBubblesMaterial() const;
	UMaterialInterface* GetOceanGerstnerWaveMaterial() const;
	UMaterialInterface* GetOceanGerstnerWaveFarMaterial() const;
	UMaterialInterface* GetOceanUnderwaterMaterial() const;
	UMaterialInterface* GetOceanUnderwaterColorlessMaterial() const;
	UMaterialInterface* GetOceanVolumetricFogMaterial() const;
	UMaterialInterface* GetOceanShorelineWetnessMaterial() const;

	UMaterialParameterCollection* GetMPC_Waves() const;
	USoundBase* GetCue_Wave_Ocean() const;
	USoundBase* GetCue_Underwater_Ocean() const;
	USoundBase* GetCue_Wave_Lake() const;
	USoundBase* GetCue_Underwater_Lake() const;
	URuntimeVirtualTexture* GetRVT_Heightmap() const;
	UVirtualTextureBuilder* GetRVT_Heightmap_Builder() const;
	UMaterialInterface* GetRT_SDF_CopyUVs() const;
	UMaterialInterface* GetRT_SDF_JumpFlood() const;
	UMaterialInterface* GetRT_SDF_ComputeSDF() const;
	UMaterialInterface* GetRT_SDF_Blur() const;
	UMaterialInterface* GetRT_SDF_Compose() const;

	URuntimeVirtualTexture* GetRVT_WaterBodies() const;
	UVirtualTextureBuilder* GetRVT_WaterBodies_Builder() const;
	UMaterialInterface* GetRVT_WaterBodies_CopyMaterial() const;
	UTextureRenderTarget2D* GetRT_WaterBodies() const;

	UNiagaraSystem* GetLakeBubblesEffect() const;
	UNiagaraSystem* GetOceanBubblesEffect() const;

	UNiagaraSystem* GetHeadBubblesEffectNiagara() const;
	UNiagaraSystem* GetRightFootBubblesEffectNiagara() const;
	UNiagaraSystem* GetLeftFootBubblesEffectNiagara() const;
	UNiagaraSystem* GetSwimFastTrailEffectNiagara() const;
	USoundBase* GetSwimFastTrailSound() const;

	UNiagaraSystem* GetNiagaraGerstnerWavesEffect() const;

	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR


#if WITH_EDITORONLY_DATA
	// Delegate called whenever the curve data is updated
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateSettings, const UOceanologyRuntimeSettings* /*Settings*/,
	                                     EPropertyChangeType::Type /*ChangeType*/);
	static FOnUpdateSettings OnSettingsChange;
#endif
};
