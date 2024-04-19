// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/OceanologyWaterParentDetailsCustomization.h"
#include "AssetToolsModule.h"
#include "BSPOps.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Builders/CubeBuilder.h"
#include "Components/BoxComponent.h"
#include "Components/Wave/OceanologyWaveSolverComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Texture2D.h"
#include "Factories/Texture2dFactoryNew.h"
#include "Utils/OceanologyMessageUtils.h"

#define LOCTEXT_NAMESPACE "OceanologyEditorWaterParent"

TSharedRef<IDetailCustomization> FOceanologyWaterParentDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FOceanologyWaterParentDetailsCustomization);
}

FOceanologyWaterParentDetailsCustomization::FOceanologyWaterParentDetailsCustomization()
{
}

FReply FOceanologyWaterParentDetailsCustomization::StartWaveSimulation()
{
	if (!OceanologyWaterParent || !OceanologyWaterParent->GetWaveSolver())
	{
		return FReply::Unhandled();
	}

	OceanologyWaterParent->GetWaveSolver()->StartWaveSimulation();

	return FReply::Handled();
}

FReply FOceanologyWaterParentDetailsCustomization::StopWaveSimulation()
{
	if (!OceanologyWaterParent || !OceanologyWaterParent->GetWaveSolver())
	{
		return FReply::Unhandled();
	}

	OceanologyWaterParent->GetWaveSolver()->StopWaveSimulation();

	return FReply::Handled();
}

void FOceanologyWaterParentDetailsCustomization::RefreshTextDetails()
{
}

void FOceanologyWaterParentDetailsCustomization::RefreshDetailsView()
{
}

void FOceanologyWaterParentDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() > 1)
	{
		return;
	}

	OceanologyWaterParent = Cast<AOceanologyWaterParent>(ObjectsBeingCustomized[0].Get());
	if (!OceanologyWaterParent)
	{
		return;
	}

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(
		"Waves",
		FText::GetEmpty(),
		ECategoryPriority::Important
	);
	TArray<TSharedRef<IPropertyHandle>> Properties;
	Category.GetDefaultProperties(Properties);

	const auto AddButtons = [&]()
	{
		Category.AddCustomRow(INVTEXT("Wave Simulation"))
		        .NameContent()[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.ContentPadding(1)
					.Text(LOCTEXT("Button_StartWaveSimulationButton", "Start Wave Simulation"))
					.OnClicked(this, &FOceanologyWaterParentDetailsCustomization::StartWaveSimulation)
				]
			]
			.ValueContent()[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.ContentPadding(1)
					.Text(LOCTEXT("Button_StopWaveSimulationButton", "Stop Wave Simulation"))
					.OnClicked(this, &FOceanologyWaterParentDetailsCustomization::StopWaveSimulation)
				]
			];
	};

	AddButtons();
}

#undef LOCTEXT_NAMESPACE
