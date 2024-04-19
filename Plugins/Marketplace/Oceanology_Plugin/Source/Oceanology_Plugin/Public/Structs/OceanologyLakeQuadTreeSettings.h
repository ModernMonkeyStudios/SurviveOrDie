#pragma once

#include "OceanologyWaterQuadTreeSettings.h"
#include "OceanologyLakeQuadTreeSettings.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyLakeQuadTreeSettings : public FOceanologyWaterQuadTreeSettings
{
	GENERATED_BODY()

	FOceanologyLakeQuadTreeSettings()
	{
		TileSize = 150;
		ExtentInTiles = FIntPoint(6, 6);
		MaxWaveHeightMultiplier = 5.0;
	}
};
