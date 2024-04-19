// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#pragma once

#include "Materials/MaterialInterface.h"
#include "PrimitiveSceneProxy.h"
#include "Materials/MaterialRelevance.h"
#include "OceanologyWaterQuadTree.h"
#include "OceanologyWaterVertexFactory.h"
#include "RayTracingGeometry.h"

class FMeshElementCollector;
struct FRayTracingMaterialGatheringContext;

class UOceanologyWaterMeshComponent;

/** Water mesh scene proxy */

class FOceanologyWaterMeshSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FOceanologyWaterMeshSceneProxy(UOceanologyWaterMeshComponent* Component);

	virtual ~FOceanologyWaterMeshSceneProxy();

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily,
	                                    uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint() const
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize() const
	{
		return (FPrimitiveSceneProxy::GetAllocatedSize() + (WaterVertexFactories.GetAllocatedSize() +
			WaterVertexFactories.Num() * sizeof(OceanologyWaterVertexFactoryType)) + WaterQuadTree.GetAllocatedSize());
	}

#if WITH_WATER_SELECTION_SUPPORT
	virtual HHitProxy*
	CreateHitProxies(UPrimitiveComponent* Component, TArray<TRefCountPtr<HHitProxy>>& OutHitProxies) override;
#endif // WITH_WATER_SELECTION_SUPPORT

	// At runtime, we only ever need one version of the vertex factory : with selection support (editor) or without : 
	using OceanologyWaterVertexFactoryType = TOceanologyWaterVertexFactory<WITH_WATER_SELECTION_SUPPORT>;
	using OceanologyWaterInstanceDataBuffersType = TOceanologyWaterInstanceDataBuffers<WITH_WATER_SELECTION_SUPPORT>;
	using OceanologyWaterMeshUserDataBuffersType = TOceanologyWaterMeshUserDataBuffers<WITH_WATER_SELECTION_SUPPORT>;

#if RHI_RAYTRACING
	virtual void GetDynamicRayTracingInstances(FRayTracingMaterialGatheringContext& Context,
	                                           TArray<FRayTracingInstance>& OutRayTracingInstances) override final;
	virtual bool HasRayTracingRepresentation() const override { return true; }
	virtual bool IsRayTracingRelevant() const override { return true; }
#endif

	void OnTessellatedWaterMeshBoundsChanged_GameThread(const FBox2D& InTessellatedWaterMeshBounds);

private:
	struct FOceanologyWaterLODParams
	{
		int32 LowestLOD;
		float HeightLODFactor;
		float WaterHeightForLOD;
	};

#if RHI_RAYTRACING
	struct FOceanologyRayTracingWaterData
	{
		FRayTracingGeometry Geometry;
		FRWBuffer DynamicVertexBuffer;
	};
#endif

	bool HasWaterData() const
	{
		return WaterQuadTree.GetNodeCount() != 0 && DensityCount != 0;
	}

	FOceanologyWaterLODParams GetWaterLODParams(const FVector& Position) const;

#if RHI_RAYTRACING
	void SetupRayTracingInstances(FRHICommandListBase& RHICmdList, int32 NumInstances, uint32 DensityIndex);
#endif

	void OnTessellatedWaterMeshBoundsChanged_RenderThread(const FBox2D& InTessellatedWaterMeshBounds);

	FMaterialRelevance MaterialRelevance;

	// One vertex factory per LOD
	TArray<OceanologyWaterVertexFactoryType*> WaterVertexFactories;

	/** Tiles containing water, stored in a quad tree */
	FOceanologyWaterQuadTree WaterQuadTree;

	/** Unique Instance data buffer shared accross water batch draw calls */
	OceanologyWaterInstanceDataBuffersType* WaterInstanceDataBuffers;

	/** Per-"water render group" user data (the number of groups might vary depending on whether we're in the editor or not) */
	OceanologyWaterMeshUserDataBuffersType* WaterMeshUserDataBuffers;

	/** The world-space bounds of the current water info texture coverage. The Water mesh should only render tiles within this bounding box. */
	FBox2D TessellatedWaterMeshBounds = FBox2D(ForceInit);

	/** Scale of the concentric LOD squares  */
	float LODScale = -1.0f;

	/** Number of densities (same as number of grid index/vertex buffers) */
	int32 DensityCount = 0;

	int32 ForceCollapseDensityLevel = TNumericLimits<int32>::Max();

	mutable int32 HistoricalMaxViewInstanceCount = 0;

#if RHI_RAYTRACING
	// Per density array of ray tracing geometries.
	TArray<TArray<FOceanologyRayTracingWaterData>> RayTracingWaterData;
#endif
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#include "Materials/Material.h"
#endif
