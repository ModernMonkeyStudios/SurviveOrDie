// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"

class URuntimeVirtualTextureComponent;

class OCEANOLOGY_PLUGIN_API FOceanologyRVTBaker
{
public:
	static void BakeRVTIntoTexture(TObjectPtr<UTexture2D>& Texture, URuntimeVirtualTextureComponent* RVTComponent);

private:
	static void BuildMips(URuntimeVirtualTextureComponent* RVTComponent, TArray64<uint8>& OutPixels, int32& OutSizeX, int32& OutSizeY);
};