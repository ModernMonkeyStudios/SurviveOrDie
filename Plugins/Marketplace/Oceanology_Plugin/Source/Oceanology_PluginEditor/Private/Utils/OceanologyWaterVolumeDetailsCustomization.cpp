// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/OceanologyWaterVolumeDetailsCustomization.h"
#include "AssetToolsModule.h"
#include "BSPOps.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Builders/CubeBuilder.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Texture2D.h"
#include "Factories/Texture2dFactoryNew.h"
#include "Utils/OceanologyMessageUtils.h"

#define LOCTEXT_NAMESPACE "OceanologyEditorWaterVolume"

TSharedRef<IDetailCustomization> FOceanologyWaterVolumeDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FOceanologyWaterVolumeDetailsCustomization);
}

FOceanologyWaterVolumeDetailsCustomization::FOceanologyWaterVolumeDetailsCustomization()
{
}

FReply FOceanologyWaterVolumeDetailsCustomization::SetVolumeBounds()
{
	if (!OceanologyWaterVolume
		|| !OceanologyWaterVolume->OceanologyWater
		|| !OceanologyWaterVolume->BrushBuilder
	)
	{
		UOceanologyMessageUtils::CreateMessage(
			OceanologyWaterVolume,
			"Either Volume, or Water or BrushBuilder is not configured!", false
		);

		return FReply::Unhandled();
	}

	OceanologyWaterVolume->PreEditChange(nullptr);

	UCubeBuilder* CubeBuilder = static_cast<UCubeBuilder*>(OceanologyWaterVolume->BrushBuilder);
	if (!CubeBuilder)
	{
		UOceanologyMessageUtils::CreateMessage(OceanologyWaterVolume,
		                                       "CubeBuilder is missing! Your shape is not a cube!", false);
		return FReply::Unhandled();
	}

	FTransform Transform;

	if (OceanologyWaterVolume->BoundsAlignActor)
	{
		GetBoundsAlignActorTransforms(
			OceanologyWaterVolume->BoundsAlignActor.Get(),
			Transform
		);
	}
	else if (OceanologyWaterVolume->OceanologyWater)
	{
		FVector Location = OceanologyWaterVolume->OceanologyWater->GetActorLocation();
		Location.Z = Location.Z * 0.7;
		Transform.SetLocation(Location);
		// Transform.SetRotation(OceanologyWaterVolume->OceanologyWater->GetActorRotation());
		Transform.SetScale3D(
			OceanologyWaterVolume->OceanologyWater->UnderwaterBoxVolumeComponent->GetScaledBoxExtent() * 2
		);
	}
	else
	{
		return FReply::Unhandled();
	}

	CubeBuilder->X = Transform.GetScale3D().X;
	CubeBuilder->Y = Transform.GetScale3D().Y;
	CubeBuilder->Z = FMath::Clamp(Transform.GetScale3D().Z, 1000, 15000);

	OceanologyWaterVolume->BrushBuilder->Build(OceanologyWaterVolume->GetWorld(), OceanologyWaterVolume);

	FBSPOps::csgPrepMovingBrush(OceanologyWaterVolume);

	if (OceanologyWaterVolume->Brush)
	{
		for (int32 i = 0; i < OceanologyWaterVolume->Brush->Polys->Element.Num(); ++i)
		{
			FPoly* Poly = &(OceanologyWaterVolume->Brush->Polys->Element[i]);
			Poly->Material = nullptr;
		}
	}


	OceanologyWaterVolume->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());

	OceanologyWaterVolume->PostEditChange();

	OceanologyWaterVolume->Modify();

	UOceanologyMessageUtils::CreateMessage(OceanologyWaterVolume, "Volume size configured.", true);

	return FReply::Handled();
}


void FOceanologyWaterVolumeDetailsCustomization::RefreshTextDetails()
{
}

void FOceanologyWaterVolumeDetailsCustomization::RefreshDetailsView()
{
}

void FOceanologyWaterVolumeDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() > 1)
	{
		return;
	}

	OceanologyWaterVolume = Cast<AOceanologyWaterVolume>(ObjectsBeingCustomized[0].Get());
	if (!OceanologyWaterVolume)
	{
		return;
	}

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(
		"Settings",
		FText::GetEmpty(),
		ECategoryPriority::Important
	);
	TArray<TSharedRef<IPropertyHandle>> Properties;
	Category.GetDefaultProperties(Properties);

	const auto AddButtons = [&]()
	{
		Category.AddCustomRow(INVTEXT("Volume Size"))
		        .NameContent()
			[
				SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("Button_SetVolumeBoundsButton", "Set Volume Bounds"))
			.ToolTipText(LOCTEXT("Button_SetVolumeBoundsButton_Tooltip",
			                     "Sets the volume size based on the water body."))
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
				.Text(LOCTEXT("Button_SetVolumeBoundsButton", "Set Volume Bounds"))
				.OnClicked(this, &FOceanologyWaterVolumeDetailsCustomization::SetVolumeBounds)
				]
			];
	};

	AddButtons();
}

void FOceanologyWaterVolumeDetailsCustomization::GetBoundsAlignActorTransforms(
	AActor* BoundsAlignActor,
	FTransform& OutTransform
)
{
	if (!BoundsAlignActor)
	{
		return;
	}

	const FVector TargetPosition = BoundsAlignActor->ActorToWorld().GetTranslation();
	const FQuat TargetRotation = BoundsAlignActor->GetTransform().GetRotation();

	FTransform LocalTransform;
	LocalTransform.SetComponents(TargetRotation, TargetPosition, FVector::OneVector);
	FTransform WorldToLocal = LocalTransform.Inverse();

	FBox Bounds(ForceInit);
	for (TObjectIterator<UPrimitiveComponent> It(RF_ClassDefaultObject, true, EInternalObjectFlags::Garbage); It; ++It)
	{
		bool bUseBounds = It->GetOwner() == BoundsAlignActor;

		if (bUseBounds)
		{
			FBoxSphereBounds LocalSpaceBounds = It->CalcBounds(It->GetComponentTransform() * WorldToLocal);
			if (LocalSpaceBounds.GetBox().GetVolume() > 0.f)
			{
				Bounds += LocalSpaceBounds.GetBox();
			}
		}
	}

	// Calculate the transform to fit the bounds.
	FTransform Transform;
	const FVector WorldSize = Bounds.GetSize();

	FVector ActorLocation = BoundsAlignActor->GetActorLocation();
	Transform.SetComponents(
		TargetRotation,
		FVector(ActorLocation.X, ActorLocation.Y, ActorLocation.Z * 0.6),
		WorldSize
	);

	OutTransform = Transform;
}

#undef LOCTEXT_NAMESPACE
