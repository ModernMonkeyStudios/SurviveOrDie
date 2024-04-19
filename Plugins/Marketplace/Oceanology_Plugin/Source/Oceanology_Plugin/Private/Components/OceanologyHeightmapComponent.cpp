// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OceanologyHeightmapComponent.h"
#include "Actors/OceanologyInfiniteOceanActor.h"
#include "Actors/OceanologyManagerActor.h"
#include "Landscape.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Async/TaskGraphInterfaces.h"
#include "Components/BoxComponent.h"
#include "Components/RuntimeVirtualTextureComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Utils/OceanologyMessageUtils.h"
#include "Utils/OceanologyRVTBaker.h"
#include "VT/VirtualTexture.h"
#include "VT/RuntimeVirtualTexture.h"
#include "VT/VirtualTextureBuilder.h"

UOceanologyHeightmapComponent::UOceanologyHeightmapComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UOceanologyHeightmapComponent::SaveHeightmap(UTexture2D* NewHeightmap)
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());

#if WITH_EDITOR
	if (!OwnerActor->IsRVTSupported())
	{
		return;
	}

	if (NewHeightmap == nullptr
		|| OwnerActor->HeightmapVirtualTextureComponent->GetVirtualTexture() == nullptr
		|| (!bUseCustomRVTBaker && OwnerActor->HeightmapVirtualTextureComponent->GetStreamingTexture() == nullptr))
	{
		UOceanologyMessageUtils::CreateMessage(this, "One of the textures is not set! RVT actor will not work!", false);
		return;
	}

	const AOceanologyInfiniteOcean* Ocean = Cast<AOceanologyInfiniteOcean>(
		UGameplayStatics::GetActorOfClass(this, AOceanologyInfiniteOcean::StaticClass()));
	if (!Ocean)
	{
		UOceanologyMessageUtils::CreateMessage(this, "There is no infinite ocean in the scene.", false);
		return;
	}

	OwnerActor->SetLocationAndScaleInWavesMPC();

	UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
		this,
		!bUseCustomRVTBaker
			? OwnerActor->HeightmapVirtualTextureComponent->GetStreamingTexture()->Texture->GetSizeX()
			: 128,
		!bUseCustomRVTBaker
			? OwnerActor->HeightmapVirtualTextureComponent->GetStreamingTexture()->Texture->GetSizeY()
			: 128
	);

	BakeHeightmapIntoRenderTarget(RenderTarget);

	UKismetRenderingLibrary::ConvertRenderTargetToTexture2DEditorOnly(this, RenderTarget, NewHeightmap);

	// We want precision for heightmap
	NewHeightmap->CompressionSettings = TC_HDR;
	NewHeightmap->LODGroup = TEXTUREGROUP_16BitData;
	NewHeightmap->AddToRoot();
	NewHeightmap->UpdateResource();
	NewHeightmap->Modify();

	HeightmapTexture = NewHeightmap;

	UOceanologyMessageUtils::CreateMessage(this, "Heightmap is saved.", true);
#endif
}

void UOceanologyHeightmapComponent::SaveHeightmapRenderTarget()
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());

	if (!OwnerActor->IsRVTSupported())
	{
		return;
	}

	if (!HeightmapRenderTarget)
	{
		UOceanologyMessageUtils::CreateMessage(this, "Heightmap Render Target not selected.", false);
		return;
	}

	if (!OwnerActor->HeightmapVirtualTextureComponent->GetVirtualTexture())
	{
		UOceanologyMessageUtils::CreateMessage(this, "RVT not assigned.", false);
		return;
	}

	const AOceanologyInfiniteOcean* Ocean = Cast<AOceanologyInfiniteOcean>(
		UGameplayStatics::GetActorOfClass(this, AOceanologyInfiniteOcean::StaticClass()));
	if (!Ocean)
	{
		UOceanologyMessageUtils::CreateMessage(this, "There is no infinite ocean in the scene.", false);
		return;
	}

	OwnerActor->SetLocationAndScaleInWavesMPC();

	BakeHeightmapIntoRenderTarget(HeightmapRenderTarget);

	UOceanologyMessageUtils::CreateMessage(this, "Heightmap is saved.", true);
}

void UOceanologyHeightmapComponent::BakeHeightmapIntoRenderTarget(UTextureRenderTarget2D* Target)
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());

	Target->AddressY = TA_Clamp;
	Target->AddressX = TA_Clamp;
	Target->RenderTargetFormat = RTF_RGBA16f;

	const AOceanologyInfiniteOcean* Ocean = Cast<AOceanologyInfiniteOcean>(
		UGameplayStatics::GetActorOfClass(this, AOceanologyInfiniteOcean::StaticClass()));
	if (!Ocean)
	{
		UOceanologyMessageUtils::CreateMessage(this, "There is no infinite ocean in the scene.", false);
		return;
	}

	UTexture2D* TextureSource = OwnerActor->HeightmapVirtualTextureComponent->GetStreamingTexture()->Texture;
	if (bUseCustomRVTBaker)
	{
		FOceanologyRVTBaker::BakeRVTIntoTexture(BakedHeightmap, OwnerActor->HeightmapVirtualTextureComponent);
		if (!BakedHeightmap)
		{
			UOceanologyMessageUtils::CreateMessage(this, "Failed to bake RVT!", false);
			return;
		}

		TextureSource = BakedHeightmap;

		UKismetRenderingLibrary::ResizeRenderTarget2D(
			Target,
			TextureSource->GetSizeX(),
			TextureSource->GetSizeY()
		);
	}

	BakeSDFIntoRenderTarget(TextureSource, Target, Ocean->GetActorLocation().Z);
}

void UOceanologyHeightmapComponent::BakeSDFIntoRenderTarget(UTexture* Heightmap, UTextureRenderTarget2D* Target,
                                                            const float OceanHeightZ)
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());

	if (const UTexture2D* HeightmapT = Cast<UTexture2D>(Heightmap))
	{
		UKismetRenderingLibrary::ResizeRenderTarget2D(
			Target,
			HeightmapT->GetSizeX(),
			HeightmapT->GetSizeY());
	}
	else if (const UTextureRenderTarget2D* HeightmapRT = Cast<UTextureRenderTarget2D>(Heightmap))
	{
		UKismetRenderingLibrary::ResizeRenderTarget2D(
			Target,
			HeightmapRT->SizeX,
			HeightmapRT->SizeY);
	}

	Target->AddressY = TA_Clamp;
	Target->AddressX = TA_Clamp;
	Target->RenderTargetFormat = RTF_RGBA16f;

	UKismetRenderingLibrary::ClearRenderTarget2D(this, Target);

	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();
	UMaterialInterface* CopyUVsMaterial = OceanologyRuntimeSettings->GetRT_SDF_CopyUVs();
	if (!CopyUVsMaterial)
	{
		UOceanologyMessageUtils::CreateMessage(this, "SDF Copy UVs material is not valid!", false);
		return;
	}

	UMaterialInterface* JumpFloodMaterial = OceanologyRuntimeSettings->GetRT_SDF_JumpFlood();
	if (!JumpFloodMaterial)
	{
		UOceanologyMessageUtils::CreateMessage(this, "SDF Jump Flood material is not valid!", false);
		return;
	}

	UMaterialInterface* ComputeSDFMaterial = OceanologyRuntimeSettings->GetRT_SDF_ComputeSDF();
	if (!ComputeSDFMaterial)
	{
		UOceanologyMessageUtils::CreateMessage(this, "SDF Compute material is not valid!", false);
		return;
	}

	UMaterialInterface* BlurMaterial = OceanologyRuntimeSettings->GetRT_SDF_Blur();
	if (!BlurMaterial)
	{
		UOceanologyMessageUtils::CreateMessage(this, "SDF Blur material is not valid!", false);
		return;
	}

	UMaterialInterface* ComposeMaterial = OceanologyRuntimeSettings->GetRT_SDF_Compose();
	if (!ComposeMaterial)
	{
		UOceanologyMessageUtils::CreateMessage(this, "SDF Compose material is not valid!", false);
		return;
	}

	if (!CopyUVsMaterialInstance ||
		CopyUVsMaterialInstance->GetMaterial() != CopyUVsMaterial)
	{
		CopyUVsMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
			this,
			CopyUVsMaterial
		);
	}

	if (!JumpFloodMaterialInstance ||
		JumpFloodMaterialInstance->GetMaterial() != JumpFloodMaterial)
	{
		JumpFloodMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
			this,
			JumpFloodMaterial
		);
	}

	if (!ComputeSDFMaterialInstance ||
		ComputeSDFMaterialInstance->GetMaterial() != ComputeSDFMaterial)
	{
		ComputeSDFMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
			this,
			ComputeSDFMaterial
		);
	}

	if (!BlurMaterialInstance ||
		BlurMaterialInstance->GetMaterial() != BlurMaterial)
	{
		BlurMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
			this,
			BlurMaterial
		);
	}

	if (!ComposeMaterialInstance ||
		ComposeMaterialInstance->GetMaterial() != ComposeMaterial)
	{
		ComposeMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
			this,
			ComposeMaterial
		);
	}

	const FVector2D Size = FVector2D(Target->SizeX, Target->SizeY);

	UTextureRenderTarget2D* TempWaterRTWrite = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, Size.X, Size.Y, RTF_RG16f);
	UTextureRenderTarget2D* TempWaterRTRead = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, Size.X, Size.Y, RTF_RG16f);
	UTextureRenderTarget2D* TempLandRTWrite = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, Size.X, Size.Y, RTF_RG16f);
	UTextureRenderTarget2D* TempLandRTRead = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, Size.X, Size.Y, RTF_RG16f);

	// Preparing offsets
	{
		CopyUVsMaterialInstance->SetScalarParameterValue("MinWaterZ", OwnerActor->GetActorLocation().Z);
		CopyUVsMaterialInstance->SetScalarParameterValue("MaxWaterZ",
		                                                 OwnerActor->GetActorLocation().Z + OwnerActor->GetActorScale().
		                                                 Z);
		CopyUVsMaterialInstance->SetScalarParameterValue("WaterHeight", OceanHeightZ);
		CopyUVsMaterialInstance->SetTextureParameterValue("Texture", Heightmap);

		CopyUVsMaterialInstance->SetScalarParameterValue("WritingLand", 0.f);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TempWaterRTWrite, CopyUVsMaterialInstance);
		CopyUVsMaterialInstance->SetScalarParameterValue("WritingLand", 1.f);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TempLandRTWrite, CopyUVsMaterialInstance);

		Swap(TempWaterRTWrite, TempWaterRTRead);
		Swap(TempLandRTWrite, TempLandRTRead);
	}

	// Jump Flood
	FVector2D Step = Size;
	const int32 Passes = FMath::CeilLogTwo(Size.X) + 1;
	for (int32 Index = 0; Index < Passes; Index++)
	{
		if (Index == 0)
		{
			JumpFloodMaterialInstance->SetVectorParameterValue("Step", FLinearColor(1.f, 1.f, 0.f, 0.f));
		}
		else
		{
			JumpFloodMaterialInstance->SetVectorParameterValue("Step", FLinearColor(Step.X, Step.Y, 0.f, 0.f));
		}
		JumpFloodMaterialInstance->SetTextureParameterValue("WaterTexture", TempWaterRTRead);
		JumpFloodMaterialInstance->SetTextureParameterValue("LandTexture", TempLandRTRead);

		JumpFloodMaterialInstance->SetScalarParameterValue("WritingLand", 0.f);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TempWaterRTWrite, JumpFloodMaterialInstance);
		JumpFloodMaterialInstance->SetScalarParameterValue("WritingLand", 1.f);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TempLandRTWrite, JumpFloodMaterialInstance);

		// We want to swap temporary render targets
		Swap(TempWaterRTWrite, TempWaterRTRead);
		Swap(TempLandRTWrite, TempLandRTRead);

		// Dividing step by 2
		Step /= 2.f;
	}

	UTextureRenderTarget2D* TempSDF = TempWaterRTWrite;

	// Compute SDF
	ComputeSDFMaterialInstance->SetTextureParameterValue("WaterTexture", TempWaterRTRead);
	ComputeSDFMaterialInstance->SetTextureParameterValue("LandTexture", TempLandRTRead);
	ComputeSDFMaterialInstance->SetTextureParameterValue("Heightmap", Heightmap);
	ComputeSDFMaterialInstance->SetScalarParameterValue("MinWaterZ", OwnerActor->GetActorLocation().Z);
	ComputeSDFMaterialInstance->SetScalarParameterValue("MaxWaterZ",
	                                                    OwnerActor->GetActorLocation().Z + OwnerActor->GetActorScale().
	                                                    Z);
	ComputeSDFMaterialInstance->SetScalarParameterValue("WaterHeight", OceanHeightZ);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TempSDF, ComputeSDFMaterialInstance);

	UTextureRenderTarget2D* TempComposedResult = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, Size.X, Size.Y, RTF_RGBA16f);
	ComposeMaterialInstance->SetTextureParameterValue("SDF", TempSDF);
	ComposeMaterialInstance->SetTextureParameterValue("Heightmap", Heightmap);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TempComposedResult, ComposeMaterialInstance);

	BlurMaterialInstance->SetTextureParameterValue("Texture", TempComposedResult);
	BlurMaterialInstance->SetScalarParameterValue("WriteAlpha", 1.f);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, Target, BlurMaterialInstance);
}


void UOceanologyHeightmapComponent::SetupLandscapeOrGroundMesh()
{
#if WITH_EDITOR
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());
	AActor* BoundsAlignActor = OwnerActor->HeightmapVirtualTextureComponent->GetBoundsAlignActor().Get();
	if (!BoundsAlignActor)
	{
		UOceanologyMessageUtils::CreateMessage(this, "You must setup the Bounds Align Actor!", false);
		return;
	}

	bool OceanologyRVTFound = false;

	if (const AStaticMeshActor* GroundMesh = Cast<AStaticMeshActor>(BoundsAlignActor))
	{
		UStaticMeshComponent* StaticMeshComponent = GroundMesh->GetStaticMeshComponent();

		for (const URuntimeVirtualTexture* RuntimeVirtualTexture : StaticMeshComponent->RuntimeVirtualTextures)
		{
			if (RuntimeVirtualTexture == OwnerActor->HeightmapVirtualTextureComponent->GetVirtualTexture())
			{
				OceanologyRVTFound = true;
			}
		}

		if (OceanologyRVTFound)
		{
			UOceanologyMessageUtils::CreateMessage(this, "Your ground mesh is already configured.", true);
			return;
		}

		{
			FProperty* VirtualTextureProperty = ALandscape::StaticClass()->FindPropertyByName(
				GET_MEMBER_NAME_CHECKED(ALandscape, RuntimeVirtualTextures)
			);
			StaticMeshComponent->PreEditChange(VirtualTextureProperty);

			StaticMeshComponent->RuntimeVirtualTextures.Add(
				OwnerActor->HeightmapVirtualTextureComponent->GetVirtualTexture());

			FPropertyChangedEvent Event(VirtualTextureProperty);
			StaticMeshComponent->PostEditChangeProperty(Event);
			StaticMeshComponent->Modify();
		}

		UOceanologyMessageUtils::CreateMessage(this, "Ground Mesh successfully configured for Oceanology.", true);
	}
	else if (ALandscape* Landscape = Cast<ALandscape>(BoundsAlignActor))
	{
		for (const URuntimeVirtualTexture* RuntimeVirtualTexture : Landscape->RuntimeVirtualTextures)
		{
			if (RuntimeVirtualTexture == OwnerActor->HeightmapVirtualTextureComponent->GetVirtualTexture())
			{
				OceanologyRVTFound = true;
			}
		}

		if (OceanologyRVTFound)
		{
			UOceanologyMessageUtils::CreateMessage(this, "Your landscape is already configured.", true);
			return;
		}

		{
			FProperty* VirtualTextureProperty = ALandscape::StaticClass()->FindPropertyByName(
				GET_MEMBER_NAME_CHECKED(ALandscape, RuntimeVirtualTextures)
			);
			Landscape->PreEditChange(VirtualTextureProperty);

			Landscape->RuntimeVirtualTextures.Add(OwnerActor->HeightmapVirtualTextureComponent->GetVirtualTexture());

			FPropertyChangedEvent Event(VirtualTextureProperty);
			Landscape->PostEditChangeProperty(Event);
			Landscape->Modify();
		}

		UOceanologyMessageUtils::CreateMessage(
			this,
			"Landscape successfully configured for Oceanology.",
			true
		);
	}
	else
	{
		UOceanologyMessageUtils::CreateMessage(
			this,
			"You must assign Bounds Align Actor either a Landscape or a StaticMeshActor!",
			false
		);
	}
#endif
}

void UOceanologyHeightmapComponent::SetupWater()
{
	TArray<AActor*> WaterActors;
	UGameplayStatics::GetAllActorsOfClass(this, AOceanologyWaterParent::StaticClass(), WaterActors);

	for (AActor* WaterActor : WaterActors)
	{
		if (!WaterActor->IsA(AOceanologyWaterParent::StaticClass()))
		{
			continue;
		}

		AOceanologyWaterParent* OceanologyWaterParent = Cast<AOceanologyWaterParent>(WaterActor);
#if WITH_EDITOR
		OceanologyWaterParent->PreEditChange(nullptr);
#endif
		if (bUseRenderTarget)
		{
			OceanologyWaterParent->RVT.T_Heightmap = HeightmapRenderTarget;
		}
		else
		{
			OceanologyWaterParent->RVT.T_Heightmap = HeightmapTexture;
		}
#if WITH_EDITOR
		OceanologyWaterParent->PostEditChange();
#endif
		OceanologyWaterParent->Modify();
	}

	UOceanologyMessageUtils::CreateMessage(this, "Heightmap has been assigned to all water actors in your level.",
	                                       true);
}

void UOceanologyHeightmapComponent::SetupWaterMinMax()
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());
	TArray<AActor*> WaterActors;
	UGameplayStatics::GetAllActorsOfClass(this, AOceanologyWaterParent::StaticClass(), WaterActors);

	for (AActor* WaterActor : WaterActors)
	{
		if (!WaterActor->IsA(AOceanologyWaterParent::StaticClass()))
		{
			continue;
		}

		AOceanologyWaterParent* OceanologyWaterParent = Cast<AOceanologyWaterParent>(WaterActor);
#if WITH_EDITOR
		OceanologyWaterParent->PreEditChange(nullptr);
#endif
		OceanologyWaterParent->RVT.WaterZMin = OwnerActor->GetActorLocation().Z;
		OceanologyWaterParent->RVT.WaterZMax = OwnerActor->GetActorLocation().Z + OwnerActor->GetActorScale().Z;
#if WITH_EDITOR
		OceanologyWaterParent->PostEditChange();
#endif
		OceanologyWaterParent->Modify();
	}

	UOceanologyMessageUtils::CreateMessage(this, "Water Z Min/Max was set to all water actors.", true);
}

void UOceanologyHeightmapComponent::Init()
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());
	OwnerActor->HeightmapVirtualTextureComponent->SetBuildLevels(BuildLevels);
#if WITH_EDITORONLY_DATA
	OwnerActor->UpdateTextureSize(OwnerActor->HeightmapVirtualTextureComponent, TextureSize);
#endif
}
