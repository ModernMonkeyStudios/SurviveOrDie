// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#pragma once

#include "Components/MeshComponent.h"
#include "OceanologyWaterQuadTree.h"
#include "Structs/OceanologyWaterQuadTreeSettings.h"
#include "OceanologyWaterMeshComponent.generated.h"

struct FPSOPrecacheParams;

/**
 * Water Mesh Component responsible for generating and rendering a continuous water mesh on top of all the existing water body actors in the world
 * The component contains a quadtree which defines where there are water tiles. A function for traversing the quadtree and outputing a list of instance data for each tile to be rendered from a point of view is included
 */
UCLASS(ClassGroup = (Rendering, Water),
	hidecategories = (Object, Activation, "Components|Activation", Collision, Lighting, HLOD, Navigation, Replication,
		Input, MaterialParameters, TextureStreaming, Tags, AssetUserData, Cooking), editinlinenew)
class OCEANOLOGY_PLUGIN_API UOceanologyWaterMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:
	UOceanologyWaterMeshComponent();

	UPROPERTY(EditAnywhere, Category = QuadTree)
	FOceanologyWaterQuadTreeSettings QuadTreeSettings;

	UPROPERTY(Transient, NonPIEDuplicateTransient, TextExportTransient)
	TObjectPtr<UMaterialInstanceDynamic> WaterMaterialInstance = nullptr;

	UPROPERTY(Transient, NonPIEDuplicateTransient, TextExportTransient)
	TObjectPtr<UMaterialInstanceDynamic> WaterFarDistanceMaterialInstance = nullptr;

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	//~ End UObject Interface

	//~ Begin UMeshComponent Interface
	virtual int32 GetNumMaterials() const override { return 0; }
	//~ End UMeshComponent Interface

	//~ Begin UPrimitiveComponent Interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials,
	                              bool bGetDebugMaterials = false) const override;
	virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
	//~ End UPrimitiveComponent Interface

	//~ Begin INavRelevantInterface Interface
	virtual bool IsNavigationRelevant() const override { return false; }
	//~ End INavRelevantInterface Interface

	virtual void CollectPSOPrecacheData(const FPSOPrecacheParams& BasePrecachePSOParams,
	                                    FComponentPSOPrecacheParamsList& OutParams) override;

	virtual void BeginPlay() override;
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

	virtual void TickComponent(
		const float DeltaTime,
		const ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	void Update();

	/** Use this instead of GetMaterialRelevance, since this one will go over all materials from all tiles */
	FMaterialRelevance GetWaterMaterialRelevance(ERHIFeatureLevel::Type InFeatureLevel) const;

	void PushTessellatedWaterMeshBoundsToPoxy(const FBox2D& TessellatedWaterMeshBounds);

	const FOceanologyWaterQuadTree& GetWaterQuadTree() const { return WaterQuadTree; }

	const TSet<TObjectPtr<UMaterialInterface>>& GetUsedMaterialsSet() const { return UsedMaterials; }

	void MarkWaterMeshGridDirty() { bNeedsRebuild = true; }

	int32 GetTessellationFactor() const
	{
		return FMath::Clamp(QuadTreeSettings.TessellationFactor + TessFactorBiasScalability, 1, 12);
	}

	float GetLODScale() const { return QuadTreeSettings.LODScale + LODScaleBiasScalability; }

	void SetExtentInTiles(FIntPoint NewExtentInTiles);
	FIntPoint GetExtentInTiles() const { return QuadTreeSettings.ExtentInTiles; }

	void SetTileSize(float NewTileSize);
	float GetTileSize() const { return QuadTreeSettings.TileSize; }

	UFUNCTION(BlueprintPure, Category = QuadTree)
	bool IsEnabled() const { return bIsEnabled; }

	void SetMaxWaveHeight(double InMaxWaveHeight);
	double GetMaxWaveHeight() const { return MaxWaveHeight; }

private:
	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface

	/** Based on all water bodies in the scene, rebuild the water mesh */
	void RebuildWaterMesh(float InTileSize, const FIntPoint& InExtentInTiles);

	/** Tiles containing water, stored in a quad tree */
	FOceanologyWaterQuadTree WaterQuadTree;

	/** Unique list of materials used by this component */
	UPROPERTY(Transient, NonPIEDuplicateTransient, TextExportTransient)
	TSet<TObjectPtr<UMaterialInterface>> UsedMaterials;

	/** Dirty flag which will make sure the water mesh is updated properly */
	bool bNeedsRebuild = true;

	/** If the system is enabled */
	bool bIsEnabled = false;

	/** Cached CVarOceanologyWaterMeshLODCountBias to detect changes in scalability */
	int32 LODCountBiasScalability = 0;

	/** Cached CVarOceanologyWaterMeshTessFactorBias to detect changes in scalability */
	int32 TessFactorBiasScalability = 0;

	/** Cached CVarOceanologyWaterMeshLODScaleBias to detect changes in scalability */
	float LODScaleBiasScalability = 0.0f;

#if WITH_EDITOR
	//~ Begin USceneComponent Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ Begin USceneComponent Interface
#endif

private:
	int32 ForceUpdateCounter = 0;
	double MaxWaveHeight = 0.0;
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#endif
