#pragma once

#include "OceanologyUnderwaterMode.generated.h"

UENUM(BlueprintType)
enum class EOceanologyUnderwaterMode : uint8
{
	None,
	Underwater,
	VolumetricFog
};
