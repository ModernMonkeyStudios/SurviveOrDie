// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "OceanologyWaterUIStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Styling/AppStyle.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"


#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define IMAGE_BRUSH_SVG(RelativePath, ...) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)


TSharedPtr<FSlateStyleSet> FOceanologyWaterUIStyle::StyleSet = nullptr;
TSharedPtr<class ISlateStyle> FOceanologyWaterUIStyle::Get() { return StyleSet; }

FName FOceanologyWaterUIStyle::GetStyleSetName()
{
	static FName WaterStyleName(TEXT("OceanologyWaterUIStyle"));
	return WaterStyleName;
}

void FOceanologyWaterUIStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);
	const FVector2D Icon128x128(128.0f, 128.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("Oceanology_Plugin"))->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("ClassIcon.OceanologyLakePreset", new IMAGE_BRUSH(TEXT("LakePreset128"), Icon20x20));
	StyleSet->Set("ClassThumbnail.OceanologyLakePreset", new IMAGE_BRUSH(TEXT("LakePreset128"), Icon128x128));

	StyleSet->Set("ClassIcon.OceanologyOceanPreset", new IMAGE_BRUSH(TEXT("OceanPreset128"), Icon20x20));
	StyleSet->Set("ClassThumbnail.OceanologyOceanPreset", new IMAGE_BRUSH(TEXT("OceanPreset128"), Icon128x128));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

#undef IMAGE_BRUSH_SVG
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef DEFAULT_FONT

void FOceanologyWaterUIStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}
