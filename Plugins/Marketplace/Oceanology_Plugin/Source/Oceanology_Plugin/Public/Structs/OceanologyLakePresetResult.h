#pragma once

#include "Structs/OceanologyGroundCaustics.h"
#include "OceanologyLakePresetResult.generated.h"

// has to be in SYNC with OceanologyLakePreset fields
USTRUCT(BlueprintType)
struct FOceanologyLakePresetResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caustics")
	FOceanologyGroundCaustics GroundCaustics;
};
