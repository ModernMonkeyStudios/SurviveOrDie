// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "OceanologyWaterParentActor.h"
#include "Enums/OceanologyWaterPresetMode.h"
#include "Structs/OceanologyGroundCaustics.h"
#include "Structs/OceanologyLakeQuadTreeSettings.h"
#include "Structs/OceanologyWaterProjection.h"
#include "UObject/Object.h"
#include "OceanologyLakeActor.generated.h"

class UOceanologyLakePreset;
class UStaticMesh;
class UStaticMeshComponent;
class UDecalComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UOceanAudioComponent;
class APawn;

UCLASS(Blueprintable)
class OCEANOLOGY_PLUGIN_API AOceanologyLake : public AOceanologyWaterParent
{
	GENERATED_BODY()

public:
	AOceanologyLake();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UOceanAudioComponent> OceanAudioUnderwaterComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UOceanAudioComponent> OceanAudioWaveComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UDecalComponent> CausticsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = QuadTree)
	FOceanologyLakeQuadTreeSettings QuadTreeSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
	TObjectPtr<UOceanologyLakePreset> Preset = nullptr;

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
		"GroundCaustics",
		"GlobalDisplacement",
		"BaseOffset",
		"Wave_1",
		"Wave_2",
		"Wave_3",
		"Wave_4",
		"Summarize",
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caustics")
	bool EnableCausticsOnGround = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caustics", Replicated)
	FOceanologyGroundCaustics GroundCaustics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caustics", Replicated)
	FOceanologyWaterProjection WaterProjection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caustics")
	TObjectPtr<UMaterialInterface> CausticsMaterial = nullptr;

	virtual TSubclassOf<UOceanologyWaveSolverComponent> GetWaveSolverClass() override;
	virtual void Init() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(Category= "Lake")
	void InitCaustics();

	UFUNCTION(Category= "Lake")
	void InitAudio();

	UFUNCTION(BlueprintCallable, Category= "Lake")
	void LoadPreset(UOceanologyLakePreset* InPreset);

	UFUNCTION()
	void RegisterUnderwaterOverlapEvents();

	UFUNCTION()
	void OnUnderwater_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                               const FHitResult& SweepResult);
	UFUNCTION()
	void OnUnderwater_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual FOceanologyWaterQuadTreeSettings GetQuadTreeSettings() override;

	virtual UMaterialInterface* GetDefaultMaterial() override;
	virtual UMaterialInterface* GetDefaultFarMaterial() override;
	virtual UMaterialInterface* GetUnderwaterMaterial() override;
	virtual UMaterialInterface* GetUnderwaterColorlessMaterial() override;
	virtual UMaterialInterface* GetVolumetricFogMaterial() override;
	virtual UMaterialInterface* GetBubblesMaterial() override;
	virtual UNiagaraSystem* GetBubblesEffect() override;

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateCausticsMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	virtual UMaterialInstanceDynamic* GetCausticsMID() override;
	
	virtual int32 GetDefaultWaterVolumeOverlapPriority() override;
	virtual FName GetGameTimeInSecondsVariableName() override;

private:
	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	UMaterialInstanceDynamic* CausticsMID;
};
