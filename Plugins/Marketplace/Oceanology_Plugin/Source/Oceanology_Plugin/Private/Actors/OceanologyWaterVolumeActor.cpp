// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Actors/OceanologyWaterVolumeActor.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/BrushComponent.h"
#include "Components/OceanBuoyancyComponent.h"
#include "Components/OceanSwimmingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Utils/OceanologyMessageUtils.h"

AOceanologyWaterVolume::AOceanologyWaterVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bEnableAutoLODGeneration = false;
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

void AOceanologyWaterVolume::Init()
{
	if (HasActorBegunPlay())
	{
		GetBrushComponent()->SetVisibility(DebugEnabled, false);
		GetBrushComponent()->SetHiddenInGame(!DebugEnabled);
	}

#if WITH_EDITORONLY_DATA
	if (!GetBrushBuilder())
	{
		UOceanologyMessageUtils::CreateMessage(
			this,
			GetName() +
			"'s BrushBuilder is missing! Please delete the actor and place it again in the level of your scene!",
			false
		);
		return;
	}
#endif

	if (!OceanologyWater)
	{
		UOceanologyMessageUtils::CreateMessage(this, "OceanologyWater is not set! The water volume will NOT work!",
		                                       false);
		return;
	}

	// user settings compatibility
	bWaterVolume = EnableSwimmingInArea;
	TerminalVelocity = PhysicsVolumeTerminalVelocity;
	Priority = PhysicsVolumePriority > 0
		           ? PhysicsVolumePriority
		           : OceanologyWater->GetDefaultWaterVolumeOverlapPriority();
	FluidFriction = PhysicsVolumeFluidFriction;

	RegisterOverlapEvents();
}

void AOceanologyWaterVolume::RegisterOverlapEvents()
{
	if (GetBrushComponent() == nullptr || !HasAuthority())
	{
		return;
	}

	GetBrushComponent()->OnComponentBeginOverlap.Clear();
	GetBrushComponent()->OnComponentBeginOverlap.AddDynamic(this, &AOceanologyWaterVolume::OnVolume_BeginOverlap);

	GetBrushComponent()->OnComponentEndOverlap.Clear();
	GetBrushComponent()->OnComponentEndOverlap.AddDynamic(this, &AOceanologyWaterVolume::OnVolume_EndOverlap);
}

void AOceanologyWaterVolume::InitialOverlappingCheck()
{
	if (GetBrushComponent() == nullptr || !CheckInitialOverlapOnBeginPlay || !HasAuthority())
	{
		return;
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), Actors);

	for (const AActor* Actor : Actors)
	{
		if (GetBrushComponent()->IsOverlappingActor(Actor))
		{
			Server_ChangeOverlapStatus(Actor, true);
		}
	}
}

void AOceanologyWaterVolume::OnVolume_BeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (HasAuthority())
	{
		Server_ChangeOverlapStatus(OtherActor, true);
	}
}

void AOceanologyWaterVolume::OnVolume_EndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (HasAuthority())
	{
		Server_ChangeOverlapStatus(OtherActor, false);
	}
}

void AOceanologyWaterVolume::Server_ChangeOverlapStatus_Implementation(
	const AActor* OtherActor,
	const bool Active
)
{
	NetMulticast_ChangeOverlapStatus(OtherActor, Active);
}

void AOceanologyWaterVolume::NetMulticast_ChangeOverlapStatus_Implementation(
	const AActor* OtherActor,
	const bool Active
)
{
	if (!OtherActor || !OceanologyWater)
	{
		// World Partition might cause an actor to disappear.
		return;
	}

	if (UOceanBuoyancyComponent* OceanBuoyancyComponent = OtherActor->GetComponentByClass<UOceanBuoyancyComponent>())
	{
		if (OceanBuoyancyComponent->OceanologyWaterVolume
			&& OceanBuoyancyComponent->OceanologyWaterVolume->Priority > Priority)
		{
			if (DebugEnabled)
			{
				UKismetSystemLibrary::PrintString(
					this,
					"Buoyancy overlap event of '" + GetName() +
					"' is skipped. Previous water volume ' " + OceanBuoyancyComponent->OceanologyWaterVolume->GetName()
					+ " ' has stronger priority!",
					true,
					true,
					FLinearColor::Yellow,
					1.0
				);
			}
			return;
		}

		OceanBuoyancyComponent->OceanologyWater = OceanologyWater;
		OceanBuoyancyComponent->OceanologyWaterVolume = this;
		OceanBuoyancyComponent->EnteredWaterVolume = Active && EnableBuoyancyInArea;

		if (OceanBuoyancyComponent->EnteredWaterVolume)
		{
			OceanBuoyancyComponent->Activate(true);
		}
		else
		{
			OceanBuoyancyComponent->Deactivate();
		}
	}

	if (UOceanSwimmingComponent* OceanSwimmingComponent = OtherActor->GetComponentByClass<UOceanSwimmingComponent>())
	{
		if (OceanSwimmingComponent->OceanologyWaterVolume
			&& OceanSwimmingComponent->OceanologyWaterVolume->Priority > Priority)
		{
			if (DebugEnabled)
			{
				UKismetSystemLibrary::PrintString(
					this,
					"Swimming overlap event of '" + GetName() +
					"' is skipped. Previous water volume ' " + OceanSwimmingComponent->OceanologyWaterVolume->GetName()
					+ " ' has stronger priority!",
					true,
					true,
					FLinearColor::Yellow,
					1.0
				);
			}

			return;
		}

		OceanSwimmingComponent->OceanologyWater = OceanologyWater;
		OceanSwimmingComponent->OceanologyWaterVolume = this;
		OceanSwimmingComponent->EnteredWaterVolume = Active && EnableSwimmingInArea;

		if (OceanSwimmingComponent->EnteredWaterVolume)
		{
			OceanSwimmingComponent->Activate(true);
		}
		else
		{
			OceanSwimmingComponent->Deactivate();
		}
	}

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			OtherActor->GetName() + " OverlapChange - Active: " + UKismetStringLibrary::Conv_BoolToString(Active),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	if (OceanologyWater->DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			OceanologyWater,
			OtherActor->GetName() + " changed his water actor to: " + OceanologyWater->GetName(),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void AOceanologyWaterVolume::BeginPlay()
{
	Super::BeginPlay();

	Init();

	if (HasAuthority() && CheckInitialOverlapOnBeginPlay)
	{
		FTimerHandle UniqueHandle;
		const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
			this,
			&AOceanologyWaterVolume::InitialOverlappingCheck
		);

		GetWorldTimerManager().SetTimer(
			UniqueHandle,
			TimerDelegate,
			InitialOverlapOnBeginPlayDelay,
			false
		);
	}
}

void AOceanologyWaterVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority())
	{
		GetBrushComponent()->OnComponentBeginOverlap.Clear();
		GetBrushComponent()->OnComponentEndOverlap.Clear();
	}
}

void AOceanologyWaterVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Init();
}

#if WITH_EDITOR
void AOceanologyWaterVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AOceanologyWaterVolume, OceanologyWater)
		&& OceanologyWater)
	{
		UOceanologyMessageUtils::CreateMessage(this, "OceanologyWater configured successfully!", true);
	}
}
#endif

void AOceanologyWaterVolume::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOceanologyWaterVolume, OceanologyWater);
}
