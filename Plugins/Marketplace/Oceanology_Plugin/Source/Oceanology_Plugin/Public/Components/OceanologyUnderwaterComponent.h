// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/OceanologyUnderwaterMode.h"
#include "Structs/OceanologyBubblesSettings.h"
#include "Structs/OceanologyUnderwater.h"
#include "Structs/OceanologyVolumetricFog.h"
#include "Structs/OceanologyMaskUnderwater.h"
#include "Structs/OceanologyWaterProjection.h"
#include "OceanologyUnderwaterComponent.generated.h"


class UPostProcessComponent;
class UBoxComponent;
class UNiagaraSystem;
class AOceanologyWaterParent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),
	hidecategories = (Object, Activation, "Components|Activation", Collision, Lighting, HLOD, Navigation, Replication,
		Input, MaterialParameters, TextureStreaming, Tags, AssetUserData, Cooking))
class OCEANOLOGY_PLUGIN_API UOceanologyUnderwaterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanologyUnderwaterComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", Replicated)
	EOceanologyUnderwaterMode UnderwaterMode = EOceanologyUnderwaterMode::Underwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater",
		meta = (EditCondition = "UnderwaterMode == EOceanologyUnderwaterMode::Underwater"), Replicated)
	FOceanologyUnderwater Underwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater|God Rays",
		meta = (EditCondition = "UnderwaterMode == EOceanologyUnderwaterMode::VolumetricFog"), Replicated)
	FOceanologyVolumetricFog VolumetricFog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", Replicated)
	FOceanologyMaskUnderwater MaskUnderwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", Replicated)
	FOceanologyWaterProjection WaterProjection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Underwater")
	bool UserOverrideUnderwaterMaterial = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater",
		meta = (EditCondition = "UserOverrideUnderwaterMaterial"))
	UMaterialInterface* UnderwaterMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater",
		meta = (EditCondition = "UserOverrideUnderwaterMaterial"))
	UMaterialInterface* VolumetricFogMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Underwater|Bubbles Settings", Replicated)
	FOceanologyBubblesSettings BubblesSettings;

	UFUNCTION(BlueprintCallable, Category="Underwater")
	void Init();

	UFUNCTION(BlueprintCallable, Category="Underwater")
	void InitBubbles();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateUnderwaterMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	UMaterialInstanceDynamic* GetUnderwaterMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateVolumetricFogMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	UMaterialInstanceDynamic* GetVolumetricFogMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	void CreateOrUpdateBubblesMID();

	UFUNCTION(BlueprintCallable, Category="Material Instance Dynamic")
	UMaterialInstanceDynamic* GetBubblesMID();

private:
	UPROPERTY(Replicated)
	TObjectPtr<AOceanologyWaterParent> OceanologyWater;

	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	UMaterialInstanceDynamic* UnderwaterMID = nullptr;

	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	UMaterialInstanceDynamic* VolumetricFogMID = nullptr;

	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient, NonPIEDuplicateTransient,
		TextExportTransient)
	UMaterialInstanceDynamic* BubblesMID = nullptr;
};
