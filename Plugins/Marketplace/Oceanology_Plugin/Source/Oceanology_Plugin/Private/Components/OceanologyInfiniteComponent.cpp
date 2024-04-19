// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Components/OceanologyInfiniteComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UOceanologyInfiniteComponent::UOceanologyInfiniteComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;

	Origin = GetWorld();
}

void UOceanologyInfiniteComponent::BeginPlay()
{
	Super::BeginPlay();

	Origin = GetWorld();
}

void UOceanologyInfiniteComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Follow(false);
}

void UOceanologyInfiniteComponent::ForceFollow()
{
	Follow(true);
}

void UOceanologyInfiniteComponent::Follow(const bool ForceFollow)
{
	if (!IsActive()
		|| ParentActor == nullptr
		|| Origin == nullptr
		|| !EnableInfinity)
	{
		return;
	}

#if WITH_EDITOR
	if (GIsEditor)
	{
		if (!EnableInfinityInEditor && !ForceFollow)
		{
			return;
		}

		TArray<FVector> ViewLocationsRenderedLastFrame = Origin->ViewLocationsRenderedLastFrame;
		const FVector CameraLocation = ViewLocationsRenderedLastFrame.IsEmpty()
			                               ? FVector::ZeroVector
			                               : ViewLocationsRenderedLastFrame[0];
		SetLocation(CameraLocation);
		return;
	}
#endif

	if (!EnableInfinityInGame && !ForceFollow)
	{
		return;
	}

	if (Origin->WorldType == EWorldType::Game || Origin->WorldType == EWorldType::PIE)
	{
		if (!UGameplayStatics::GetPlayerController(Origin, 0))
			return;

		if (!UGameplayStatics::GetPlayerController(Origin, 0)->PlayerCameraManager)
			return;

		FVector CameraLocation;
		FRotator CameraRotation;

		UGameplayStatics::GetPlayerController(Origin, 0)->PlayerCameraManager->GetCameraViewPoint(
			CameraLocation,
			CameraRotation
		);

		SetLocation(CameraLocation);
	}
}

void UOceanologyInfiniteComponent::Init(AActor* Actor, UWorld* InOrigin)
{
	SetParentActor(Actor);
	SetOrigin(InOrigin);

	SetComponentTickInterval(FollowUpdateInterval);
}

void UOceanologyInfiniteComponent::SetOrigin(UWorld* InOrigin)
{
	Origin = InOrigin;

	if (EnableDebug)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Origin set to: " + Origin->GetName(),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanologyInfiniteComponent::InitDefaultOrigin()
{
	SetOrigin(GetWorld());
}

void UOceanologyInfiniteComponent::SetParentActor(AActor* Actor)
{
	ParentActor = Actor;

	if (EnableDebug)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"ParentActor set to: " + ParentActor->GetName(),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanologyInfiniteComponent::SetLocation(const FVector& CameraLocation)
{
	FVector NewParentLocation = CameraLocation.GridSnap(TimeJump);
	if (TimeJump > 0 && NewParentLocation == CameraLocation)
	{
		return;
	}

	if (NewParentLocation == LastLocation)
	{
		return;
	}

	if (EnableDebug)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"NewParentLocation: " + NewParentLocation.ToString() + ", CameraLocation: " + CameraLocation.ToString(),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	NewParentLocation.Z = ParentActor->GetActorLocation().Z;
	ParentActor->SetActorLocation(NewParentLocation);

	OnInfiniteLocationChanged.Broadcast(CameraLocation, LastLocation, NewParentLocation);

	LastLocation = NewParentLocation;
}
