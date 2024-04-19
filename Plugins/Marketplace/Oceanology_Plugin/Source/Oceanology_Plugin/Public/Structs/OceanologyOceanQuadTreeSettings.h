#pragma once

#include "OceanologyWaterQuadTreeSettings.h"
#include "OceanologyOceanQuadTreeSettings.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyOceanQuadTreeSettings : public FOceanologyWaterQuadTreeSettings
{
	GENERATED_BODY()

	FOceanologyOceanQuadTreeSettings()
	{
		bUseFarMesh = true;
		FarDistanceMeshExtent = 200000.0;
		MaxWaveHeightMultiplier = 5000.0;
	}
};
