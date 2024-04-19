// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Enums/OceanologyWaterPresetMode.h"
#include "GameFramework/Actor.h"
#include "Structs/OceanologyActorHeight.h"
#include "Structs/OceanologyCaustics.h"
#include "Structs/OceanologyFlipbook.h"
#include "Structs/OceanologyFoam.h"
#include "Structs/OceanologyFolding.h"
#include "Structs/OceanologyHorizonCorrection.h"
#include "Structs/OceanologyProcedural.h"
#include "Structs/OceanologyRefraction.h"
#include "Structs/OceanologyRVT.h"
#include "Structs/OceanologySurfaceScattering.h"
#include "Structs/OceanologyMask.h"
#include "Structs/OceanologyGGX.h"
#include "Structs/OceanologyWaterQuadTreeSettings.h"
#include "OceanologyWaterParentActor.generated.h"

class UOceanologyWaterMeshComponent;
class UOceanologyWaveSolverComponent;
class UOceanologyWaterPreset;
class UNiagaraSystem;
class USceneComponent;
class UBoxComponent;
class UPostProcessComponent;
class UNiagaraComponent;
class UStaticMeshComponent;
class UOceanologyUnderwaterComponent;
class AOceanologyInfiniteOcean;

UCLASS(Abstract)
class OCEANOLOGY_PLUGIN_API AOceanologyWaterParent : public AActor
{
	GENERATED_BODY()

public:
	AOceanologyWaterParent();

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "Default"))
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UOceanologyWaterMeshComponent> WaterMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UBoxComponent> UnderwaterBoxVolumeComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UPostProcessComponent> UnderwaterPostProcessComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UOceanologyUnderwaterComponent> UnderwaterComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UStaticMeshComponent> VolumetricFogMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default"))
	TObjectPtr<UNiagaraComponent> BubblesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	TSubclassOf<UOceanologyWaveSolverComponent> WaveSolverClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Waves")
	UOceanologyWaveSolverComponent* WaveSolver = nullptr;

	/** Turning on/off debug messages of the water. WARNING: Debugging affects performance, do NOT use in production! */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Water Parent|Debug")
	bool DebugEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Surface Scattering", Replicated)
	FOceanologySurfaceScattering SurfaceScattering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Caustics", Replicated)
	FOceanologyCaustics Caustics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Refraction", Replicated)
	FOceanologyRefraction Refraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Normal Calculation", Replicated)
	FOceanologyHorizonCorrection HorizonCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Surface|Normal Calculation", Replicated)
	FOceanologyFlipbook Flipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam", Replicated)
	FOceanologyFoam Foam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Foam|Folding", Replicated)
	FOceanologyFolding Folding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural", Replicated)
	FOceanologyProcedural Procedural;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RVT", Replicated)
	FOceanologyRVT RVT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask", Replicated)
	FOceanologyMask Mask;

	UPROPERTY(Replicated)
	FOceanologyActorHeight ActorHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GGX", Replicated)
	FOceanologyGGX GGX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	bool UserOverrideMaterial = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material",
		meta = (EditCondition = "UserOverrideMaterial"))
	TObjectPtr<UMaterialInterface> Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material",
		meta = (EditCondition = "UserOverrideMaterial"))
	TObjectPtr<UMaterialInterface> MaterialFar = nullptr;

	UFUNCTION(CallInEditor, Category = "Default", BlueprintCallable)
	void ReloadSettings();

	virtual void Init();

	UFUNCTION(BlueprintNativeEvent, Category="Water Surface")
	void InitSurface();

	UFUNCTION(Category= "Waves")
	virtual TSubclassOf<UOceanologyWaveSolverComponent> GetWaveSolverClass();

	UFUNCTION(Category= "Waves")
	bool SetupWaveSolvers();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	virtual void SetVectorParameterValue(const FName ParameterName, const FLinearColor Value);

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	virtual void SetScalarParameterValue(const FName ParameterName, const double Value);

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	virtual void SetTextureParameterValue(const FName ParameterName, UTexture* Value);

	UFUNCTION(BlueprintCallable, Category="Water Material Parameters")
	void SetWaterMaterialParameters(UMaterialInstanceDynamic* MID) const;

	virtual bool ShouldTickIfViewportsOnly() const override
	{
		return true;
	}

#if WITH_EDITOR
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif

	virtual FOceanologyWaterQuadTreeSettings GetQuadTreeSettings();

	virtual UMaterialInterface* GetDefaultMaterial();
	virtual UMaterialInterface* GetDefaultFarMaterial();
	virtual UMaterialInterface* GetUnderwaterMaterial();
	virtual UMaterialInterface* GetUnderwaterColorlessMaterial();
	virtual UMaterialInterface* GetVolumetricFogMaterial();
	virtual UMaterialInterface* GetBubblesMaterial();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateWaterMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	UMaterialInstanceDynamic* GetWaterMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateWaterFarDistanceMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	UMaterialInstanceDynamic* GetWaterFarDistanceMID();

	virtual UMaterialInstanceDynamic* GetCausticsMID();

	virtual UNiagaraSystem* GetBubblesEffect();

	UFUNCTION(BlueprintCallable, Category="Waves")
	UOceanologyWaveSolverComponent* GetWaveSolver() const;

	UFUNCTION(
		BlueprintCallable, BlueprintPure = false,
		Category="Waves",
		meta = (
			DeterminesOutputType = "InWaveSolverClass",
			DynamicOutputParam = "OutWaveSolver",
			ExpandBoolAsExecs = "ReturnValue"
		)
	)
	bool GetValidWaveSolverByClass(
		UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UOceanologyWaveSolverComponent> InWaveSolverClass,
		UOceanologyWaveSolverComponent*& OutWaveSolver
	) const;

	UFUNCTION(BlueprintPure, Category="Water Surface")
	virtual FVector GetWaveHeightAtLocation(const FVector& Location);

	UFUNCTION(BlueprintPure, Category="Water Surface")
	double GetWaterDensity(const AActor* Actor);
	
	virtual int32 GetDefaultWaterVolumeOverlapPriority();
	virtual FName GetGameTimeInSecondsVariableName();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void OnWaveSolverPropertyChanged();

	void OnPresetPropertyChanged(
		const EOceanologyWaterPresetMode PresetMode,
		UOceanologyWaterPreset* Preset,
		const TArray<FName>& PresetInclusionGroups
	);
#endif

protected:
	bool WaveSolverSetup = false;

private:
	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	TObjectPtr<UMaterialInstanceDynamic> WaterMID;

	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	TObjectPtr<UMaterialInstanceDynamic> WaterFarDistanceMID;
};
