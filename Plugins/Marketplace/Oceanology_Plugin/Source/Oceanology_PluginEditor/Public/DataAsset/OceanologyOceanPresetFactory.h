// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "OceanologyOceanPresetFactory.generated.h"

UCLASS()
class OCEANOLOGY_PLUGINEDITOR_API UOceanologyOceanPresetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UOceanologyOceanPresetFactory();

	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	virtual FString GetDefaultNewAssetName() const override;
	// End UFactory Interface
};
