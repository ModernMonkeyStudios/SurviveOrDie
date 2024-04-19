// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Visibility.h"
#include "Actors/OceanologyWaterVolumeActor.h"

class UTexture2D;
class IPropertyHandle;
class STextBlock;

class OCEANOLOGY_PLUGINEDITOR_API FOceanologyWaterVolumeDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

protected:
	FOceanologyWaterVolumeDetailsCustomization();

	FReply SetVolumeBounds();

	/** Callback for updating text values after an edit. */
	void RefreshTextDetails();
	/** Callback for full update of details view after an edit. */
	void RefreshDetailsView();

	//~ Begin IDetailCustomization Interface.
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//~ End IDetailCustomization Interface.

private:
	AOceanologyWaterVolume* OceanologyWaterVolume = nullptr;

	void GetBoundsAlignActorTransforms(
		AActor* BoundsAlignActor,
		FTransform& OutTransform
	);
};
