#pragma once

#include "OceanologyWaterPresetMode.generated.h"

UENUM(BlueprintType)
enum class EOceanologyWaterPresetMode : uint8
{
	UsePresetDirectly,
	ApplyPresetToCurrentSettings
};
