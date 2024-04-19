#pragma once

#include "OceanologyWaterQuadTreeSettings.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyWaterQuadTreeSettings
{
	GENERATED_BODY()

	/** World size of the water tiles at LOD0. Multiply this with the ExtentInTiles to get the world extents of the system */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "100"))
	float TileSize = 2400.0f;

	/** The extent of the system in number of tiles. Maximum number of tiles for this system will be ExtentInTiles.X*2*ExtentInTiles.Y*2 */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "1"))
	FIntPoint ExtentInTiles = FIntPoint(64, 64);

	/** Highest tessellation factor of a water tile. Max number of verts on the side of a tile will be (2^TessellationFactor)+1)  */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "1", ClampMax = "12"))
	int32 TessellationFactor = 6;

	/** World scale of the concentric LODs */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "0.5"))
	float LODScale = 1.0f;

	/** The count of force updates necessary after certain re-render requests. (Scale change, location change, and so.) */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "1", ClampMax = "20"))
	int ForceUpdateCount = 10;

	/** The interval to check for updates whether quad tree re-render requests are available and necessary. 0 = real time checking on event tick, but NOT real-time re-render. Re-render happens on-demand. */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "0", ClampMax = "1.0"))
	float CheckForUpdateInterval = 0;

	/** At above what density level a tile is allowed to force collapse even if not all leaf nodes in the subtree are present.
	 *	Collapsing will not occus if any child node in the subtree has different materials.
	 *	Setting this to -1 means no collapsing is allowed and the water mesh will always keep it's silhouette at any distance.
	 *	Setting this to 0 will allow every level to collapse
	 *	Setting this to something higher than the LODCount will have no effect
	 */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "-1"))
	int32 ForceCollapseDensityLevel = -1;

	/** Used to multiply the Min and Max bounds on Z axis of the quad tree actor's quad tree mesh bounds. Bigger your waves are, the more you need to increase this value to prevent your water being frustum culled. */
	UPROPERTY(EditAnywhere, Category = QuadTree, meta = (ClampMin = "1"))
	double MaxWaveHeightMultiplier = 1.0;

	/** Forces the water mesh to always render the far mesh, regardless if there is an ocean or not.*/
	UPROPERTY(Category = "QuadTree|FarDistance", EditAnywhere)
	bool bUseFarMesh = false;

	UPROPERTY(EditAnywhere, Category = "QuadTree|FarDistance", meta = (ClampMin = "0"))
	float FarDistanceMeshExtent = 2400.0f;

	UPROPERTY(EditAnywhere, Category = "QuadTree|FarDistance")
	float FarDistanceCenterMeshExtent = 1.0f;

	UPROPERTY(EditAnywhere, Category = "QuadTree|FarDistance")
	float FarDistanceMeshHeightOffset = -10.0f;

	FOceanologyWaterQuadTreeSettings()
	{
	}
};
