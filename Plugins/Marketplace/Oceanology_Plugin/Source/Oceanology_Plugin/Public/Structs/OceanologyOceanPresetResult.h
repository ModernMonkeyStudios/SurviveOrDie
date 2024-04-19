#pragma once

#include "OceanologyWetness.h"
#include "OceanologyOceanPresetResult.generated.h"

// has to be in SYNC with OceanologyOceanPreset fields
USTRUCT(BlueprintType)
struct FOceanologyOceanPresetResult
{
	GENERATED_BODY()
	
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness")
	// bool EnableWetness = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline Wetness")
	FOceanologyWetness Wetness;
};
