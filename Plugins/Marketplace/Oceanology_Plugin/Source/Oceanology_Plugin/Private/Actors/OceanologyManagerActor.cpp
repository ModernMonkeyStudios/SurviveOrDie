// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Actors/OceanologyManagerActor.h"
#include "Actors/OceanologyInfiniteOceanActor.h"
#include "Landscape.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Async/TaskGraphInterfaces.h"
#include "Components/BoxComponent.h"
#include "Components/OceanologyHeightmapComponent.h"
#include "Components/OceanologyWaterBodiesComponent.h"
#include "Components/RuntimeVirtualTextureComponent.h"
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

#include UE_INLINE_GENERATED_CPP_BY_NAME(OceanologyManagerActor)

AOceanologyManager::AOceanologyManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->SetMobility(EComponentMobility::Movable);
	RootComponent = DefaultSceneRoot;
	DefaultSceneRoot->CreationMethod = EComponentCreationMethod::Native;

	HeightmapVirtualTextureComponent = CreateDefaultSubobject<UOceanologyVirtualTextureComponent>(
		TEXT("HeightmapVirtualTexture")
	);
	HeightmapVirtualTextureComponent->SetupAttachment(DefaultSceneRoot);


	HeightmapComponent = CreateDefaultSubobject<UOceanologyHeightmapComponent>(TEXT("Heightmap"));

	WaterBodiesVirtualTextureComponent = CreateDefaultSubobject<UOceanologyVirtualTextureComponent>(
		TEXT("WaterBodiesVirtualTexture")
	);
	WaterBodiesVirtualTextureComponent->SetupAttachment(DefaultSceneRoot);

	WaterBodiesComponent = CreateDefaultSubobject<UOceanologyWaterBodiesComponent>("WaterBodies");

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Box->SetupAttachment(DefaultSceneRoot);
	Box->SetBoxExtent(FVector(.5f, .5f, .5f), false);
	Box->SetRelativeTransform(FTransform(FVector(.5f, .5f, .5f)));
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Box->SetCanEverAffectNavigation(false);
	Box->CanCharacterStepUpOn = ECB_No;
	Box->SetGenerateOverlapEvents(false);

	bEnableAutoLODGeneration = false;
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

void AOceanologyManager::SetBoundsAlignActor()
{
#if WITH_EDITOR
	PreEditChange(nullptr);
	
	HeightmapVirtualTextureComponent->PreEditChange(nullptr);
	HeightmapVirtualTextureComponent->SetBoundsAlignActor(BoundsAlignActor.LoadSynchronous());
	HeightmapVirtualTextureComponent->PostEditChange();

	WaterBodiesVirtualTextureComponent->PreEditChange(nullptr);
	WaterBodiesVirtualTextureComponent->SetBoundsAlignActor(BoundsAlignActor.LoadSynchronous());
	WaterBodiesVirtualTextureComponent->PostEditChange();

	PostEditChange();
#endif
}

void AOceanologyManager::SetLocationAndScaleInWavesMPC() const
{
	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();
	UMaterialParameterCollectionInstance* MPCI = GetWorld()->GetParameterCollectionInstance(
		OceanologyRuntimeSettings->GetMPC_Waves()
	);
	MPCI->SetVectorParameterValue("Location", Box->GetComponentLocation());
	MPCI->SetVectorParameterValue("Scale", Box->GetComponentScale());
}

bool AOceanologyManager::IsRVTSupported() const
{
	static const auto CVarVirtualTexture = IConsoleManager::Get().
		FindTConsoleVariableDataInt(TEXT("r.VirtualTextures"));
	if (CVarVirtualTexture &&
		CVarVirtualTexture->GetValueOnAnyThread() == 0)
	{
		UOceanologyMessageUtils::CreateMessage(this, "Virtual Textures are disabled in the project settings.", false);
		return false;
	}
	if (!UseVirtualTexturing(GMaxRHIFeatureLevel))
	{
		UOceanologyMessageUtils::CreateMessage(this, "Virtual Textures are not supported on this platform.", false);
		return false;
	}

	return true;
}

void AOceanologyManager::UpdateTextureSize(
	const UOceanologyVirtualTextureComponent* InVirtualTextureComponent,
	int32& OutTextureSize
) const
{
#if WITH_EDITOR
	OutTextureSize = InVirtualTextureComponent->GetVirtualTexture()->GetTileSize()
		<< (InVirtualTextureComponent->NumStreamingMips() - 1);
#endif
}

void AOceanologyManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	Migrate();

	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();

	if (HeightmapVirtualTextureComponent->GetVirtualTexture() == nullptr)
	{
		HeightmapVirtualTextureComponent->SetVirtualTexture(OceanologyRuntimeSettings->GetRVT_Heightmap());
	}

	if (HeightmapVirtualTextureComponent->GetStreamingTexture() == nullptr)
	{
		HeightmapVirtualTextureComponent->SetStreamingTexture(OceanologyRuntimeSettings->GetRVT_Heightmap_Builder());
	}

#if WITH_EDITORONLY_DATA
	UpdateTextureSize(HeightmapVirtualTextureComponent, HeightmapComponent->TextureSize);
#endif
	HeightmapComponent->Init();

	if (WaterBodiesVirtualTextureComponent->GetVirtualTexture() == nullptr)
	{
		WaterBodiesVirtualTextureComponent->SetVirtualTexture(OceanologyRuntimeSettings->GetRVT_WaterBodies());
	}

	if (WaterBodiesVirtualTextureComponent->GetStreamingTexture() == nullptr)
	{
		WaterBodiesVirtualTextureComponent->SetStreamingTexture(
			OceanologyRuntimeSettings->GetRVT_WaterBodies_Builder()
		);
	}

#if WITH_EDITORONLY_DATA
	UpdateTextureSize(WaterBodiesVirtualTextureComponent, WaterBodiesComponent->TextureSize);
#endif
	WaterBodiesComponent->Init();

#endif

	SetLocationAndScaleInWavesMPC();
}

#if WITH_EDITOR
void AOceanologyManager::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	SetLocationAndScaleInWavesMPC();
}

void AOceanologyManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyManager, BoundsAlignActor))
	{
		SetBoundsAlignActor();
	}
}
#endif

void AOceanologyManager::BeginPlay()
{
	Super::BeginPlay();

	SetLocationAndScaleInWavesMPC();
}

void AOceanologyManager::Migrate()
{
#if WITH_EDITOR
	// Migrate
	if (DefaultSceneRoot && RootComponent != DefaultSceneRoot)
	{
		PreEditChange(nullptr);

		SetActorLabel(GetDefaultActorLabel());

		RootComponent = DefaultSceneRoot;
		DefaultSceneRoot->SetMobility(EComponentMobility::Movable);

		PostEditChange();

		Modify();
	}
#endif
}
