// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Components/QuadTree/OceanologyWaterMeshComponent.h"
#include "EngineUtils.h"
#include "MaterialDomain.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "PhysicsEngine/BodySetup.h"
#include "Components/QuadTree/OceanologyWaterMeshSceneProxy.h"
#include "Utils/OceanologyWaterUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OceanologyWaterMeshComponent)

/** Scalability CVars*/
static TAutoConsoleVariable<int32> CVarOceanologyWaterMeshLODCountBias(
	TEXT("r.Oceanology.WaterMesh.LODCountBias"), 0,
	TEXT(
		"This value is added to the LOD Count of each Water Mesh Component. Negative values will lower the quality(fewer and larger water tiles at the bottom level of the water quadtree), higher values will increase quality (more and smaller water tiles at the bottom level of the water quadtree)"),
	ECVF_Scalability);

static TAutoConsoleVariable<int32> CVarOceanologyWaterMeshTessFactorBias(
	TEXT("r.Oceanology.WaterMesh.TessFactorBias"), 0,
	TEXT(
		"This value is added to the tessellation factor of each Mesh Component. Negative values will lower the overall density/resolution or the vertex grid, higher values will increase the density/resolution "),
	ECVF_Scalability);

static TAutoConsoleVariable<float> CVarOceanologyWaterMeshLODScaleBias(
	TEXT("r.Oceanology.WaterMesh.LODScaleBias"), 0.0f,
	TEXT(
		"This value is added to the LOD Scale of each Mesh Component. Negative values will lower the overall density/resolution or the vertex grid and make the LODs smaller, higher values will increase the density/resolution and make the LODs larger. Smallest value is -0.5. That will make the inner LOD as tight and optimized as possible"),
	ECVF_Scalability);

/** Debug CVars */
static TAutoConsoleVariable<int32> CVarOceanologyWaterMeshShowTileGenerationGeometry(
	TEXT("r.Oceanology.WaterMesh.ShowTileGenerationGeometry"),
	0,
	TEXT("This debug option will display the geometry used for intersecting the water grid and generating tiles"),
	ECVF_Default
);

static TAutoConsoleVariable<int32> CVarOceanologyWaterMeshForceRebuildMeshPerFrame(
	TEXT("r.Oceanology.WaterMesh.ForceRebuildMeshPerFrame"),
	0,
	TEXT("Force rebuilding the entire mesh each frame"),
	ECVF_Default
);

TAutoConsoleVariable<int32> CVarOceanologyWaterMeshEnabled(
	TEXT("r.Oceanology.WaterMesh.Enabled"),
	1,
	TEXT("If the water mesh is enabled or disabled. This affects both rendering and the water tile generation"),
	ECVF_RenderThreadSafe
);

TAutoConsoleVariable<int32> CVarOceanologyWaterEnabled(
	TEXT("r.Oceanology.Water.Enabled"),
	1,
	TEXT("Water"),
	ECVF_RenderThreadSafe
);

extern TAutoConsoleVariable<float> CVarOceanologyWaterSplineResampleMaxDistance;


// ----------------------------------------------------------------------------------

UOceanologyWaterMeshComponent::UOceanologyWaterMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;
	bHasPerInstanceHitProxies = true;
}

void UOceanologyWaterMeshComponent::PostLoad()
{
	Super::PostLoad();
}

void UOceanologyWaterMeshComponent::CollectPSOPrecacheData(const FPSOPrecacheParams& BasePrecachePSOParams,
                                                           FComponentPSOPrecacheParamsList& OutParams)
{
	const FVertexFactoryType* OceanologyWaterVertexFactoryType = GetOceanologyWaterVertexFactoryType(
		/*bWithWaterSelectionSupport = */ false
	);

	for (UMaterialInterface* MaterialInterface : UsedMaterials)
	{
		if (MaterialInterface)
		{
			FComponentPSOPrecacheParams& ComponentParams = OutParams[OutParams.AddDefaulted()];
			ComponentParams.Priority = EPSOPrecachePriority::High;
			ComponentParams.MaterialInterface = MaterialInterface;
			ComponentParams.VertexFactoryDataList.Add(FPSOPrecacheVertexFactoryData(OceanologyWaterVertexFactoryType));
			ComponentParams.PSOPrecacheParams = BasePrecachePSOParams;
		}
	}
}

void UOceanologyWaterMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	ForceUpdateCounter = 0;
}

void UOceanologyWaterMeshComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	SetComponentTickInterval(QuadTreeSettings.CheckForUpdateInterval);

	ForceUpdateCounter = 0;
}

void UOceanologyWaterMeshComponent::Deactivate()
{
	Super::Deactivate();

	ForceUpdateCounter = 0;
}

void UOceanologyWaterMeshComponent::OnUpdateTransform(
	EUpdateTransformFlags UpdateTransformFlags,
	ETeleportType Teleport
)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	ForceUpdateCounter = 0;
}

void UOceanologyWaterMeshComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ForceUpdateCounter < QuadTreeSettings.ForceUpdateCount)
	{
		ForceUpdateCounter++;
		MarkWaterMeshGridDirty();
	}

	Update();
}

void UOceanologyWaterMeshComponent::PostInitProperties()
{
	Super::PostInitProperties();

	UpdateBounds();
	MarkRenderTransformDirty();
}

FPrimitiveSceneProxy* UOceanologyWaterMeshComponent::CreateSceneProxy()
{
	// Early out
	if (!bIsEnabled)
	{
		return nullptr;
	}

	return new FOceanologyWaterMeshSceneProxy(this);
}

void UOceanologyWaterMeshComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials,
                                                     bool bGetDebugMaterials) const
{
	for (UMaterialInterface* Mat : UsedMaterials)
	{
		if (Mat)
		{
			OutMaterials.Add(Mat);
		}
	}
}

void UOceanologyWaterMeshComponent::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	// UE_LOG(LogOceanology, Warning,
	//        TEXT(
	// 	       "SetMaterial is not compatible with UOceanologyWaterMeshComponent since all materials on this component are auto-populated from the Water Bodies contained within it."
	//        ));
}

FMaterialRelevance UOceanologyWaterMeshComponent::GetWaterMaterialRelevance(ERHIFeatureLevel::Type InFeatureLevel) const
{
	// Combine the material relevance for all materials.
	FMaterialRelevance Result;
	for (UMaterialInterface* Mat : UsedMaterials)
	{
		Result |= Mat->GetRelevance_Concurrent(InFeatureLevel);
	}

	return Result;
}

void UOceanologyWaterMeshComponent::PushTessellatedWaterMeshBoundsToPoxy(const FBox2D& TessellatedWaterMeshBounds)
{
	if (SceneProxy)
	{
		static_cast<FOceanologyWaterMeshSceneProxy*>(SceneProxy)->OnTessellatedWaterMeshBoundsChanged_GameThread(
			TessellatedWaterMeshBounds);
	}
}

void UOceanologyWaterMeshComponent::SetExtentInTiles(FIntPoint NewExtentInTiles)
{
	QuadTreeSettings.ExtentInTiles = NewExtentInTiles;
	MarkWaterMeshGridDirty();
	MarkRenderStateDirty();
}

void UOceanologyWaterMeshComponent::SetTileSize(float NewTileSize)
{
	QuadTreeSettings.TileSize = NewTileSize;
	MarkWaterMeshGridDirty();
	MarkRenderStateDirty();
}

void UOceanologyWaterMeshComponent::SetMaxWaveHeight(double InMaxWaveHeight)
{
	MaxWaveHeight = InMaxWaveHeight * QuadTreeSettings.MaxWaveHeightMultiplier;
}

FBoxSphereBounds UOceanologyWaterMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	// Always return valid bounds (tree is initialized with invalid bounds and if nothing is inserted, the tree bounds will stay invalid)
	FBox NewBounds = WaterQuadTree.GetBounds();

	// Add the far distance to the bounds if it's valid
	if (QuadTreeSettings.bUseFarMesh)
	{
		NewBounds = NewBounds.ExpandBy(FVector(QuadTreeSettings.FarDistanceMeshExtent,
		                                       QuadTreeSettings.FarDistanceMeshExtent, 0.0f));
	}

	return NewBounds;
}

static bool IsMaterialUsedWithWater(const UMaterialInterface* InMaterial)
{
	return (InMaterial && InMaterial->CheckMaterialUsage_Concurrent(EMaterialUsage::MATUSAGE_Water));
}

void UOceanologyWaterMeshComponent::RebuildWaterMesh(float InTileSize, const FIntPoint& InExtentInTiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RebuildWaterMesh);

	AActor* Owner = GetOwner();

	const FVector ComponentLocation = GetComponentLocation();
	const double ComponentLocationZ = ComponentLocation.Z;
	const FVector ComponentScale = GetComponentScale();

	int32 NewExtentInTilesX = FMath::FloorToInt(InExtentInTiles.X * ComponentScale.X);
	int32 NewExtentInTilesY = FMath::FloorToInt(InExtentInTiles.Y * ComponentScale.Y);

	// We must ensure that the zone is always at least 1x1
	NewExtentInTilesX = FMath::Max(1, NewExtentInTilesX);
	NewExtentInTilesY = FMath::Max(1, NewExtentInTilesY);

	const FIntPoint LocalExtentInTiles = FIntPoint(
		NewExtentInTilesX,
		NewExtentInTilesY
	);

	// Position snapped to the grid
	const FVector2D GridPosition = FVector2D(FMath::GridSnap<FVector::FReal>(ComponentLocation.X, InTileSize),
	                                         FMath::GridSnap<FVector::FReal>(ComponentLocation.Y, InTileSize));
	const FVector2D WorldExtent = FVector2D(InTileSize * LocalExtentInTiles.X, InTileSize * LocalExtentInTiles.Y);

	const FBox2D WaterWorldBox = FBox2D(-WorldExtent + GridPosition, WorldExtent + GridPosition);

	// This resets the tree to an initial state, ready for node insertion
	WaterQuadTree.InitTree(WaterWorldBox, InTileSize, LocalExtentInTiles);

	UsedMaterials.Empty();

	FOceanologyWaterBodyRenderData RenderData;

	// Assign material instance
	RenderData.Material = WaterMaterialInstance;

	if (RenderData.Material)
	{
		if (!IsMaterialUsedWithWater(RenderData.Material))
		{
			RenderData.Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
		UsedMaterials.Add(RenderData.Material);
	}

	RenderData.Priority = 0;
	RenderData.WaterBodyIndex = INDEX_NONE;
	RenderData.SurfaceBaseHeight = ComponentLocationZ;
	RenderData.WaterBodyType = 1;
#if WITH_WATER_SELECTION_SUPPORT
	RenderData.HitProxy = new HActor(Owner, nullptr);
	RenderData.bWaterBodySelected = Owner->IsSelected();
#endif

	const uint32 WaterBodyRenderDataIndex = WaterQuadTree.AddWaterBodyRenderData(RenderData);

	TRACE_CPUPROFILER_EVENT_SCOPE(QuadTreeWater);

	FBox BoundingBox = Owner->GetComponentsBoundingBox();
	BoundingBox.Min.Z = ComponentLocationZ - 5 - MaxWaveHeight;
	BoundingBox.Max.Z = ComponentLocationZ + 5 + MaxWaveHeight;
	WaterQuadTree.AddWaterTilesInsideBounds(BoundingBox, WaterBodyRenderDataIndex);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (!!CVarOceanologyWaterMeshShowTileGenerationGeometry.GetValueOnGameThread())
	{
		DrawDebugBox(GetWorld(), BoundingBox.GetCenter(), BoundingBox.GetExtent(), FColor::Red);
	}
#endif

	// Build the far distance mesh instances if needed
	if ((QuadTreeSettings.bUseFarMesh)
		&& (IsMaterialUsedWithWater(WaterFarDistanceMaterialInstance)
			&& QuadTreeSettings.FarDistanceMeshExtent > 0.0f))
	{
		UsedMaterials.Add(WaterFarDistanceMaterialInstance);

		// Place far mesh height just below the ocean level.
		const float FarMeshHeight = (RenderData.SurfaceBaseHeight) + QuadTreeSettings.FarDistanceMeshHeightOffset;

		WaterQuadTree.AddFarMesh(
			WaterFarDistanceMaterialInstance,
			QuadTreeSettings.FarDistanceMeshExtent,
			FarMeshHeight,
			QuadTreeSettings.FarDistanceCenterMeshExtent
		);
	}

	WaterQuadTree.Unlock(true);

	MarkRenderStateDirty();
}

void UOceanologyWaterMeshComponent::Update()
{
	bIsEnabled = FOceanologyWaterUtils::IsWaterMeshEnabled(/*bIsRenderThread = */false);

	// Early out
	if (!bIsEnabled)
	{
		return;
	}

	const int32 NewLODCountBias = CVarOceanologyWaterMeshLODCountBias.GetValueOnGameThread();
	const int32 NewTessFactorBias = CVarOceanologyWaterMeshTessFactorBias.GetValueOnGameThread();
	const float NewLODScaleBias = CVarOceanologyWaterMeshLODScaleBias.GetValueOnGameThread();
	if (bNeedsRebuild
		|| !!CVarOceanologyWaterMeshShowTileGenerationGeometry.GetValueOnGameThread()
		|| !!CVarOceanologyWaterMeshForceRebuildMeshPerFrame.GetValueOnGameThread()
		|| (NewLODCountBias != LODCountBiasScalability)
		|| (NewTessFactorBias != TessFactorBiasScalability)
		|| (NewLODScaleBias != LODScaleBiasScalability))
	{
		LODCountBiasScalability = NewLODCountBias;
		TessFactorBiasScalability = NewTessFactorBias;
		LODScaleBiasScalability = NewLODScaleBias;
		const float LODCountBiasFactor = FMath::Pow(2.0f, (float)LODCountBiasScalability);
		RebuildWaterMesh(QuadTreeSettings.TileSize / LODCountBiasFactor,
		                 FIntPoint(FMath::CeilToInt(QuadTreeSettings.ExtentInTiles.X * LODCountBiasFactor),
		                           FMath::CeilToInt(QuadTreeSettings.ExtentInTiles.Y * LODCountBiasFactor)));
		PrecachePSOs();
		bNeedsRebuild = false;
	}
}


#if WITH_EDITOR
void UOceanologyWaterMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* PropertyThatChanged = PropertyChangedEvent.MemberProperty;
	if (PropertyThatChanged)
	{
		const FName PropertyName = PropertyThatChanged->GetFName();

		// Properties that needs the scene proxy to be rebuilt
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UOceanologyWaterMeshComponent, QuadTreeSettings))
		{
			MarkWaterMeshGridDirty();
			MarkRenderStateDirty();
		}
	}
}
#endif
