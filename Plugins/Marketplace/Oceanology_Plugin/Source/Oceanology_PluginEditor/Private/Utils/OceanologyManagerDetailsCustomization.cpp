// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/OceanologyManagerDetailsCustomization.h"
#include "AssetToolsModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "RuntimeVirtualTextureSetBounds.h"
#include "RuntimeVirtualTextureSetBounds.h"
#include "Components/OceanologyHeightmapComponent.h"
#include "Components/OceanologyWaterBodiesComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Texture2D.h"
#include "Factories/Texture2dFactoryNew.h"

#define LOCTEXT_NAMESPACE "OceanologyEditorManager"

TAutoConsoleVariable<int32> CVarOceanologyDevSettings(
	TEXT("oceanology.dev.enable"), 0, TEXT("Enable/Disable recheck of building system weights after every action."));

TSharedRef<IDetailCustomization> FOceanologyManagerDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FOceanologyManagerDetailsCustomization);
}

FOceanologyManagerDetailsCustomization::FOceanologyManagerDetailsCustomization()
{
}

bool FOceanologyManagerDetailsCustomization::IsSetBoundsEnabled() const
{
	return OceanologyManager->HeightmapVirtualTextureComponent->GetVirtualTexture() != nullptr
		&& OceanologyManager->WaterBodiesVirtualTextureComponent->GetVirtualTexture() != nullptr;
}

FReply FOceanologyManagerDetailsCustomization::SetBounds()
{
	if (OceanologyManager->HeightmapVirtualTextureComponent->GetVirtualTexture() != nullptr
		&& OceanologyManager->WaterBodiesVirtualTextureComponent->GetVirtualTexture() != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("Transaction_SetBounds",
		                                             "Set RuntimeVirtualTextureComponent Bounds"));

		RuntimeVirtualTexture::SetBounds(OceanologyManager->HeightmapVirtualTextureComponent);
		// RuntimeVirtualTexture::SetBounds(OceanologyManager->WaterBodiesVirtualTextureComponent);
		
		// Force update of editor view widget.
		GEditor->NoteSelectionChange(false);

		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply FOceanologyManagerDetailsCustomization::SetupLandscapeOrGroundMesh()
{
	OceanologyManager->HeightmapComponent->SetupLandscapeOrGroundMesh();

	return FReply::Handled();
}

FReply FOceanologyManagerDetailsCustomization::SaveHeightmap()
{
	if (OceanologyManager->HeightmapComponent->bUseRenderTarget)
	{
		OceanologyManager->HeightmapComponent->SaveHeightmapRenderTarget();
		return FReply::Handled();
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	const FString DefaultPath = FPackageName::GetLongPackagePath(OceanologyManager->GetPathName());

	const auto CreateTexture = [&](const FString& Suffix)
	{
		const FString DefaultName = FPackageName::GetShortName(OceanologyManager->GetWorld()->GetName() + "_" + Suffix);

		UTexture2DFactoryNew* FactoryInstance = DuplicateObject<UTexture2DFactoryNew>(
			GetDefault<UTexture2DFactoryNew>(),
			GetTransientPackage()
		);

		UObject* Object = AssetToolsModule.Get().CreateAssetWithDialog(
			DefaultName,
			DefaultPath,
			UTexture2D::StaticClass(),
			FactoryInstance
		);

		return Cast<UTexture2D>(Object);
	};

	UTexture2D* Heightmap = CreateTexture("Heightmap");
	if (!Heightmap)
	{
		return FReply::Handled();
	}

	OceanologyManager->HeightmapComponent->SaveHeightmap(Heightmap);

	return FReply::Handled();
}

FReply FOceanologyManagerDetailsCustomization::SetupWater()
{
	OceanologyManager->HeightmapComponent->SetupWater();

	return FReply::Handled();
}

FReply FOceanologyManagerDetailsCustomization::SetWaterMinMax()
{
	OceanologyManager->HeightmapComponent->SetupWaterMinMax();

	return FReply::Handled();
}

FReply FOceanologyManagerDetailsCustomization::CaptureRT()
{
	OceanologyManager->WaterBodiesComponent->CaptureRT();

	return FReply::Handled();
}

void FOceanologyManagerDetailsCustomization::RefreshTextDetails()
{
}

void FOceanologyManagerDetailsCustomization::RefreshDetailsView()
{
}

void FOceanologyManagerDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	if (!CVarOceanologyDevSettings.GetValueOnAnyThread())
	{
		DetailBuilder.HideCategory("VirtualTexture");
		DetailBuilder.HideCategory("VirtualTextureBuild");
		DetailBuilder.HideCategory("TransformFromBounds");
	}

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() > 1)
	{
		return;
	}

	OceanologyManager = Cast<AOceanologyManager>(ObjectsBeingCustomized[0].Get());
	if (!OceanologyManager)
	{
		return;
	}

	EditHeightmapCategory(DetailBuilder);
	EditWaterBodiesCategory(DetailBuilder);
	EditBoundsCategory(DetailBuilder);
}

void FOceanologyManagerDetailsCustomization::EditHeightmapCategory(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Heightmap");
	TArray<TSharedRef<IPropertyHandle>> Properties;
	Category.GetDefaultProperties(Properties);

	const auto AddButtons = [&]()
	{
		Category.AddCustomRow(INVTEXT("Setup Landscape"))
		        .NameContent()
			[
				SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("Button_SetupLandscapeGroundMeshButton", "Setup Landscape/Ground Mesh"))
			.ToolTipText(LOCTEXT("Button_SetupLandscapeGroundMeshButton_Tooltip", "Save the heightmap texture"))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(2)
				.Text(LOCTEXT("Button_SetupLandscapeGroundMeshButton", "Setup Landscape/Ground Mesh"))
				.OnClicked(this, &FOceanologyManagerDetailsCustomization::SetupLandscapeOrGroundMesh)
				]
			];

		Category.AddCustomRow(INVTEXT("Save Heightmap"))
		        .NameContent()
			[
				SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("Button_SaveHeightmapButton", "Save Heightmap"))
			.ToolTipText(LOCTEXT("Button_SaveHeightmapButton_Tooltip", "Save the heightmap texture"))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(2)
				.Text(LOCTEXT("Button_SaveHeightmapButton", "Save Heightmap"))
				.OnClicked(this, &FOceanologyManagerDetailsCustomization::SaveHeightmap)
				]
			];

		Category.AddCustomRow(INVTEXT("Setup Water"))
		        .NameContent()
			[
				SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("Button_SetupWaterButton", "Setup Water"))
			.ToolTipText(LOCTEXT("Button_SetupWaterButton_Tooltip", "Save the heightmap texture"))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(2)
				.Text(LOCTEXT("Button_SetupWaterButton", "Setup Water"))
				.OnClicked(this, &FOceanologyManagerDetailsCustomization::SetupWater)
				]
			];

		Category.AddCustomRow(INVTEXT("Set Water Min/Max"))
		        .NameContent()
			[
				SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("Button_SetupWaterButton", "Set Water Min/Max"))
			.ToolTipText(LOCTEXT("Button_SetupWaterButton_Tooltip", "Set water Min/Max from volume actor"))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(2)
				.Text(LOCTEXT("Button_SetupWaterButton", "Set Water Min/Max"))
				.OnClicked(this, &FOceanologyManagerDetailsCustomization::SetWaterMinMax)
				]
			];
	};

	for (TSharedRef<IPropertyHandle> Property : Properties)
	{
		Category.AddProperty(Property);

		if (Property->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(UOceanologyHeightmapComponent, TextureSize))
		{
			AddButtons();
		}
	}
}

void FOceanologyManagerDetailsCustomization::EditWaterBodiesCategory(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("WaterBodies");
	TArray<TSharedRef<IPropertyHandle>> Properties;
	Category.GetDefaultProperties(Properties);

	const auto AddButtons = [&]()
	{
		Category.AddCustomRow(INVTEXT("Capture RT"))
		        .NameContent()
			[
				SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("Button_CaptureRT", "Capture RT"))
			.ToolTipText(LOCTEXT("Button_CaptureRT_Tooltip", "Capture the render target"))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(4.0f)
				[
					SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(2)
				.Text(LOCTEXT("Button_CaptureRTButton", "Capture RT"))
				.OnClicked(this, &FOceanologyManagerDetailsCustomization::CaptureRT)
				]
			];
	};

	for (TSharedRef<IPropertyHandle> Property : Properties)
	{
		Category.AddProperty(Property);

		if (Property->GetProperty()->GetFName() ==
			GET_MEMBER_NAME_CHECKED(UOceanologyWaterBodiesComponent, TextureSize))
		{
			AddButtons();
		}
	}
}

void FOceanologyManagerDetailsCustomization::EditBoundsCategory(IDetailLayoutBuilder& DetailBuilder)
{
	// Apply custom widget for SetBounds.
	TSharedRef<IPropertyHandle> SetBoundsPropertyHandle = DetailBuilder.GetProperty(TEXT("bSetAllBoundsButton"));
	DetailBuilder.EditDefaultProperty(SetBoundsPropertyHandle)->CustomWidget()
	             .NameContent()
		[
			SNew(STextBlock)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.Text(LOCTEXT("Button_SetBounds", "Set Bounds"))
		.ToolTipText(LOCTEXT("Button_SetBounds_Tooltip",
		                     "Set the rotation to match the Bounds Align Actor and expand bounds to include all primitives that write to this virtual texture."))
		]
		.ValueContent()
		.MinDesiredWidth(125.f)
		[
			SNew(SButton)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.ContentPadding(2)
		.Text(LOCTEXT("Button_SetBounds", "Set Bounds"))
		.OnClicked(this, &FOceanologyManagerDetailsCustomization::SetBounds)
		.IsEnabled(this, &FOceanologyManagerDetailsCustomization::IsSetBoundsEnabled)
		];
}


#undef LOCTEXT_NAMESPACE
