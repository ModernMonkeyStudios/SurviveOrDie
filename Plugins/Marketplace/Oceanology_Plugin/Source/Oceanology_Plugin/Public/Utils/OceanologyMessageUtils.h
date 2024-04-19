// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanologyMessageUtils.generated.h"

UCLASS()
class OCEANOLOGY_PLUGIN_API UOceanologyMessageUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Oceanology Messages", meta=(WorldContext="WorldContextObject"))
	static void CreateMessage(
		const UObject* WorldContextObject,
		const FString& Message,
		const bool bSuccess
	);
};
