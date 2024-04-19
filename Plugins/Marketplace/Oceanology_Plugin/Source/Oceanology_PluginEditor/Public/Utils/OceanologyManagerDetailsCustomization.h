// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Visibility.h"
#include "Actors/OceanologyManagerActor.h"

class UTexture2D;
class IPropertyHandle;
class STextBlock;

class OCEANOLOGY_PLUGINEDITOR_API FOceanologyManagerDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

protected:
	FOceanologyManagerDetailsCustomization();

	bool IsSetBoundsEnabled() const;
	FReply SetBounds();

	FReply SetupLandscapeOrGroundMesh();
	FReply SaveHeightmap();
	FReply SetupWater();
	FReply SetWaterMinMax();

	FReply CaptureRT();

	/** Callback for updating text values after an edit. */
	void RefreshTextDetails();
	/** Callback for full update of details view after an edit. */
	void RefreshDetailsView();

	//~ Begin IDetailCustomization Interface.
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//~ End IDetailCustomization Interface.

	void EditHeightmapCategory(IDetailLayoutBuilder& DetailBuilder);
	void EditWaterBodiesCategory(IDetailLayoutBuilder& DetailBuilder);
	void EditBoundsCategory(IDetailLayoutBuilder& DetailBuilder);

private:
	AOceanologyManager* OceanologyManager = nullptr;
};
