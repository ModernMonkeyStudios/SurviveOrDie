// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OceanologyWaterBodiesComponent.h"
#include "Engine/World.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyManagerActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Utils/OceanologyMessageUtils.h"
#include "Utils/OceanologyRVTBaker.h"
#include "VT/VirtualTextureBuilder.h"
#include "VT/VirtualTexture.h"

UOceanologyWaterBodiesComponent::UOceanologyWaterBodiesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOceanologyWaterBodiesComponent::Init()
{
	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();
	if (!WaterBodiesRenderTarget)
	{
		WaterBodiesRenderTarget = OceanologyRuntimeSettings->GetRT_WaterBodies();
	}
	
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());
	OwnerActor->WaterBodiesVirtualTextureComponent->SetBuildLevels(BuildLevels);
#if WITH_EDITORONLY_DATA
	OwnerActor->UpdateTextureSize(OwnerActor->WaterBodiesVirtualTextureComponent, TextureSize);
#endif
}

void UOceanologyWaterBodiesComponent::CaptureRT()
{
	AOceanologyManager* OwnerActor = Cast<AOceanologyManager>(GetOwner());
	if (!WaterBodiesRenderTarget)
	{
		return;
	}

	WaterBodiesRenderTarget->AddressY = TA_Clamp;
	WaterBodiesRenderTarget->AddressX = TA_Clamp;
	WaterBodiesRenderTarget->RenderTargetFormat = RTF_RGBA16f;

	UTexture2D* TextureSource = OwnerActor->HeightmapVirtualTextureComponent->GetStreamingTexture()->Texture;
	if (bUseCustomRVTBaker)
	{
		FOceanologyRVTBaker::BakeRVTIntoTexture(BakedWaterBodies, OwnerActor->WaterBodiesVirtualTextureComponent);
		if (!BakedWaterBodies)
		{
			UOceanologyMessageUtils::CreateMessage(this, "Failed to bake RVT!", false);
			return;
		}

		TextureSource = BakedWaterBodies;
	}

	UKismetRenderingLibrary::ResizeRenderTarget2D(
		WaterBodiesRenderTarget,
		TextureSource->GetSizeX(),
		TextureSource->GetSizeY()
	);

	if (!CopyMaterialInstance)
	{
		CopyMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
			this,
			GetDefault<UOceanologyRuntimeSettings>()->GetRVT_WaterBodies_CopyMaterial()
		);
	}

	CopyMaterialInstance->SetTextureParameterValue("Texture", TextureSource);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, WaterBodiesRenderTarget, CopyMaterialInstance);
}
