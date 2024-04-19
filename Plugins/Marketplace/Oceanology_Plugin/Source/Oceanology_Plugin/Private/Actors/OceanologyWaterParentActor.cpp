// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Actors/OceanologyWaterParentActor.h"
#include "NiagaraComponent.h"
#include "OceanologyRuntimeSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/QuadTree/OceanologyWaterMeshComponent.h"
#include "Utils/OceanologyWaterUtils.h"
#include "Components/Wave/OceanologyWaveSolverComponent.h"
#include "DataAsset/OceanologyWaterPreset.h"
#include "Components/StaticMeshComponent.h"
#include "Enums/OceanologyWaterPresetMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Structs/OceanologyActorHeight.h"
#include "Utils/OceanologyMessageUtils.h"

AOceanologyWaterParent::AOceanologyWaterParent()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->SetMobility(EComponentMobility::Movable);
	RootComponent = DefaultSceneRoot;
	DefaultSceneRoot->CreationMethod = EComponentCreationMethod::Native;

	WaterMeshComponent = CreateDefaultSubobject<UOceanologyWaterMeshComponent>("WaterMesh");
	WaterMeshComponent->SetMobility(EComponentMobility::Movable);
	WaterMeshComponent->SetupAttachment(DefaultSceneRoot);

	UnderwaterBoxVolumeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("UnderwaterBoxVolume"));
	UnderwaterBoxVolumeComponent->SetMobility(EComponentMobility::Movable);
	UnderwaterBoxVolumeComponent->SetupAttachment(DefaultSceneRoot);

	UnderwaterPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("UnderwaterPostProcess"));
	UnderwaterPostProcessComponent->SetMobility(EComponentMobility::Movable);
	UnderwaterPostProcessComponent->SetupAttachment(UnderwaterBoxVolumeComponent);
	UnderwaterBoxVolumeComponent->ShapeColor = FColor::Blue;

	VolumetricFogMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VolumetricFogMesh"));
	VolumetricFogMeshComponent->SetStaticMesh(GetDefault<UOceanologyRuntimeSettings>()->GetVolumetricFogMesh());
	VolumetricFogMeshComponent->SetMobility(EComponentMobility::Movable);
	VolumetricFogMeshComponent->SetupAttachment(DefaultSceneRoot);
	VolumetricFogMeshComponent->CanCharacterStepUpOn = ECB_No;
	VolumetricFogMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VolumetricFogMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	VolumetricFogMeshComponent->SetVisibility(false, false);
	VolumetricFogMeshComponent->SetCastShadow(false);

	UnderwaterComponent = CreateDefaultSubobject<UOceanologyUnderwaterComponent>("Underwater");
	UnderwaterComponent->SetIsReplicated(true);

	BubblesComponent = CreateDefaultSubobject<UNiagaraComponent>("Bubbles");
	BubblesComponent->SetMobility(EComponentMobility::Movable);
	BubblesComponent->SetupAttachment(DefaultSceneRoot);
	BubblesComponent->SetVisibility(false, false);

	bReplicates = true;
	bEnableAutoLODGeneration = false;
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

void AOceanologyWaterParent::ReloadSettings()
{
	Init();

	UOceanologyMessageUtils::CreateMessage(this, "Settings reloaded successfully!", true);
}

void AOceanologyWaterParent::Init()
{
	SetActorRotation(FRotator::ZeroRotator);

	ActorHeight.LocationOffset = FLinearColor(GetActorLocation());

	WaterMeshComponent->QuadTreeSettings = GetQuadTreeSettings();
}

void AOceanologyWaterParent::InitSurface_Implementation()
{
	if (!Material)
	{
		Material = GetDefaultMaterial();
	}

	if (!MaterialFar)
	{
		MaterialFar = GetDefaultFarMaterial();
	}

	WaterMeshComponent->WaterMaterialInstance = GetWaterMID();
	SetWaterMaterialParameters(WaterMeshComponent->WaterMaterialInstance);

	if (GetQuadTreeSettings().bUseFarMesh)
	{
		WaterMeshComponent->WaterFarDistanceMaterialInstance = GetWaterFarDistanceMID();
		SetWaterMaterialParameters(WaterMeshComponent->WaterFarDistanceMaterialInstance);
	}
}

TSubclassOf<UOceanologyWaveSolverComponent> AOceanologyWaterParent::GetWaveSolverClass()
{
	return nullptr;
}

bool AOceanologyWaterParent::SetupWaveSolvers()
{
	if (WaveSolverSetup || WaveSolver != nullptr)
	{
		return true;
	}

	if (WaveSolverClass == nullptr)
	{
		WaveSolverClass = GetWaveSolverClass();
	}

	if (WaveSolverClass == nullptr)
	{
		UOceanologyMessageUtils::CreateMessage(this, "Wave Solver Class is missing! The water will not work!", false);
		return false;
	}

	WaveSolver = NewObject<UOceanologyWaveSolverComponent>(this, WaveSolverClass.Get());
	WaveSolver->SetIsReplicated(true);

	if (WaveSolver == nullptr)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Wave Solver is missing! The water will not work!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);
		return false;
	}

	WaveSolverSetup = true;

	return true;
}

void AOceanologyWaterParent::SetVectorParameterValue(const FName ParameterName, const FLinearColor Value)
{
	GetWaterMID()->SetVectorParameterValue(ParameterName, Value);

	if (GetQuadTreeSettings().bUseFarMesh)
		GetWaterFarDistanceMID()->SetVectorParameterValue(ParameterName, Value);
}

void AOceanologyWaterParent::SetScalarParameterValue(const FName ParameterName, const double Value)
{
	GetWaterMID()->SetScalarParameterValue(ParameterName, Value);

	if (GetQuadTreeSettings().bUseFarMesh)
		GetWaterFarDistanceMID()->SetScalarParameterValue(ParameterName, Value);
}

void AOceanologyWaterParent::SetTextureParameterValue(const FName ParameterName, UTexture* Value)
{
	GetWaterMID()->SetTextureParameterValue(ParameterName, Value);

	if (GetQuadTreeSettings().bUseFarMesh)
		GetWaterFarDistanceMID()->SetTextureParameterValue(ParameterName, Value);
}

void AOceanologyWaterParent::SetWaterMaterialParameters(UMaterialInstanceDynamic* MID) const
{
	UOceanologySurfaceScatteringHelper::SetMaterialParameters(MID, SurfaceScattering);
	UOceanologyCausticsHelper::SetMaterialParameters(MID, Caustics);
	UOceanologyRefractionHelper::SetMaterialParameters(MID, Refraction);
	UOceanologyHorizonCorrectionHelper::SetMaterialParameters(MID, HorizonCorrection);
	UOceanologyFlipbookHelper::SetMaterialParameters(MID, Flipbook);
	UOceanologyFoamHelper::SetMaterialParameters(MID, Foam);
	UOceanologyFoldingHelper::SetMaterialParameters(MID, Folding);
	UOceanologyProceduralHelper::SetMaterialParameters(MID, Procedural);
	UOceanologyRVTHelper::SetMaterialParameters(MID, RVT);
	UOceanologyMaskHelper::SetMaterialParameters(MID, Mask);
	UOceanologyActorHeightHelper::SetMaterialParameters(MID, ActorHeight);
	UOceanologyGGXHelper::SetMaterialParameters(MID, GGX);
}

FOceanologyWaterQuadTreeSettings AOceanologyWaterParent::GetQuadTreeSettings()
{
	return FOceanologyWaterQuadTreeSettings();
}

UMaterialInterface* AOceanologyWaterParent::GetDefaultMaterial()
{
	return nullptr;
}

UMaterialInterface* AOceanologyWaterParent::GetDefaultFarMaterial()
{
	return nullptr;
}

UMaterialInterface* AOceanologyWaterParent::GetUnderwaterMaterial()
{
	return nullptr;
}

UMaterialInterface* AOceanologyWaterParent::GetUnderwaterColorlessMaterial()
{
	return nullptr;
}

UMaterialInterface* AOceanologyWaterParent::GetVolumetricFogMaterial()
{
	return nullptr;
}

UMaterialInterface* AOceanologyWaterParent::GetBubblesMaterial()
{
	return nullptr;
}

void AOceanologyWaterParent::CreateOrUpdateWaterMID()
{
	if (!GetWorld())
	{
		return;
	}

	WaterMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		WaterMID,
		TEXT("WaterMID"),
		Material,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* AOceanologyWaterParent::GetWaterMID()
{
	CreateOrUpdateWaterMID();

	return WaterMID;
}

void AOceanologyWaterParent::CreateOrUpdateWaterFarDistanceMID()
{
	if (!GetWorld())
	{
		return;
	}

	WaterFarDistanceMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
		WaterFarDistanceMID,
		TEXT("WaterFarDistanceMID"),
		MaterialFar,
		FOceanologyWaterUtils::GetTransientMIDFlags()
	);
}

UMaterialInstanceDynamic* AOceanologyWaterParent::GetWaterFarDistanceMID()
{
	CreateOrUpdateWaterFarDistanceMID();

	return WaterFarDistanceMID;
}

UMaterialInstanceDynamic* AOceanologyWaterParent::GetCausticsMID()
{
	return nullptr;
}

UNiagaraSystem* AOceanologyWaterParent::GetBubblesEffect()
{
	return nullptr;
}

UOceanologyWaveSolverComponent* AOceanologyWaterParent::GetWaveSolver() const
{
	return WaveSolver;
}

bool AOceanologyWaterParent::GetValidWaveSolverByClass(
	TSubclassOf<UOceanologyWaveSolverComponent> InWaveSolverClass,
	UOceanologyWaveSolverComponent*& OutWaveSolver
) const
{
	OutWaveSolver = WaveSolver;

	return OutWaveSolver != nullptr && OutWaveSolver->IsA(InWaveSolverClass);
}

FVector AOceanologyWaterParent::GetWaveHeightAtLocation(const FVector& Location)
{
	return WaveSolver->GetWaveHeightAtLocation(Location);
}

double AOceanologyWaterParent::GetWaterDensity(const AActor* Actor)
{
	const FVector ActorLocation = Actor->GetActorLocation();
	const FVector WaveHeightAtLocation = GetWaveHeightAtLocation(ActorLocation);
	const double ComputedZ = WaveHeightAtLocation.Z - ActorLocation.Z;

	return UKismetMathLibrary::MapRangeClamped(
		ComputedZ,
		40.0,
		200.0,
		6000.0,
		240.0
	);
}

int32 AOceanologyWaterParent::GetDefaultWaterVolumeOverlapPriority()
{
	return 0;
}

FName AOceanologyWaterParent::GetGameTimeInSecondsVariableName()
{
	return "";
}

void AOceanologyWaterParent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Init();
}

void AOceanologyWaterParent::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void AOceanologyWaterParent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

#if WITH_EDITOR
void AOceanologyWaterParent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyWaterParent, Material)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyWaterParent, MaterialFar))
	{
		InitSurface();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void AOceanologyWaterParent::OnWaveSolverPropertyChanged()
{
	WaveSolverSetup = false;

	if (WaveSolver)
	{
		WaveSolver->DestroyComponent();
		WaveSolver = nullptr;
	}

	Init();
}

void AOceanologyWaterParent::OnPresetPropertyChanged(
	const EOceanologyWaterPresetMode PresetMode,
	UOceanologyWaterPreset* Preset,
	const TArray<FName>& PresetInclusionGroups
)
{
	if (!Preset)
	{
		return;
	}

	Preset->OnPropertyUpdated.RemoveAll(this);

	if (PresetMode == EOceanologyWaterPresetMode::UsePresetDirectly)
	{
		UOceanologyMessageUtils::CreateMessage(this, "Direct Water Preset '" + Preset->GetPathName() + "' changed.",
		                                       true);
	}
	else if (PresetMode == EOceanologyWaterPresetMode::ApplyPresetToCurrentSettings)
	{
		Preset->LoadPreset(this, PresetInclusionGroups);
		Modify();

		UOceanologyMessageUtils::CreateMessage(this, "Water Preset '" + Preset->GetPathName() + "' settings applied.",
		                                       true);
	}

	if (WaveSolver)
	{
		WaveSolver->UpdateWaves();
	}
}
#endif

void AOceanologyWaterParent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOceanologyWaterParent, SurfaceScattering);
	DOREPLIFETIME(AOceanologyWaterParent, Caustics);
	DOREPLIFETIME(AOceanologyWaterParent, Refraction);
	DOREPLIFETIME(AOceanologyWaterParent, HorizonCorrection);
	DOREPLIFETIME(AOceanologyWaterParent, Flipbook);
	DOREPLIFETIME(AOceanologyWaterParent, Foam);
	DOREPLIFETIME(AOceanologyWaterParent, Folding);
	DOREPLIFETIME(AOceanologyWaterParent, Procedural);
	DOREPLIFETIME(AOceanologyWaterParent, RVT);
	DOREPLIFETIME(AOceanologyWaterParent, Mask);
	DOREPLIFETIME(AOceanologyWaterParent, ActorHeight);
	DOREPLIFETIME(AOceanologyWaterParent, GGX);
}
