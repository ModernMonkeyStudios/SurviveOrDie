// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/RuntimeVirtualTextureComponent.h"
#include "OceanologyManagerActor.generated.h"

class UOceanologyWaterBodiesComponent;
class UOceanologyHeightmapComponent;
class UWorld;
class UMaterialParameterCollectionInstance;
class UTextureRenderTarget2D;
class UBoxComponent;
class UVirtualTexture2D;
class UVirtualTextureBuilder;
class UTexture2D;

UCLASS(HideCategories =(AssetUserData, Activation, Tags, Cooking, Navigation))
class UOceanologyVirtualTextureComponent : public URuntimeVirtualTextureComponent
{
	GENERATED_BODY()

public:
	void SetBuildLevels(const int32 InBuildLevels)
	{
		StreamLowMips = InBuildLevels;
	}
};

UCLASS(HideCategories = (Actor, Collision, Cooking, Input, LOD, Physics, Replication, Rendering, Tags, Activation))
class OCEANOLOGY_PLUGIN_API AOceanologyManager : public AActor
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(BlueprintReadWrite, NonTransactional, Category = Default)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Heightmap, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanologyVirtualTextureComponent> HeightmapVirtualTextureComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Heightmap, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanologyHeightmapComponent> HeightmapComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = WaterBodies, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanologyVirtualTextureComponent> WaterBodiesVirtualTextureComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = WaterBodies, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOceanologyWaterBodiesComponent> WaterBodiesComponent;

	/** Actor to align rotation to. If set this actor is always included in the bounds calculation. */
	UPROPERTY(EditAnywhere, Category = Bounds)
	TSoftObjectPtr<AActor> BoundsAlignActor = nullptr;

	/** Placeholder for details customization button. */
	UPROPERTY(VisibleAnywhere, Transient, Category = Bounds)
	bool bSetAllBoundsButton;

	void SetBoundsAlignActor();

	void SetLocationAndScaleInWavesMPC() const;
	bool IsRVTSupported() const;
	void UpdateTextureSize(
		const UOceanologyVirtualTextureComponent* InVirtualTextureComponent,
		int32& OutTextureSize
	) const;

#if WITH_EDITOR
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif

protected:
	//~ Begin UObject Interface.
	virtual bool NeedsLoadForServer() const override { return false; }
	//~ End UObject Interface.
	//~ Begin AActor Interface.
	virtual bool IsLevelBoundsRelevant() const override { return false; }
	//~ End AActor Interface

	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UBoxComponent> Box;

	void Migrate();
};
