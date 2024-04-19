// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Components/OceanSwimmingComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "OceanologyRuntimeSettings.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/OceanBuoyancyComponent.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "TimerManager.h"
#include "Utils/OceanologyWaterUtils.h"

UOceanSwimmingComponent::UOceanSwimmingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
	bTickInEditor = false;
}

void UOceanSwimmingComponent::Activate(const bool bReset)
{
	SetComponentTickInterval(SwimmingUpdateInterval);

	SetupUnderwaterEffects();

	if (!GetOwner()->IsA(ACharacter::StaticClass()))
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Owner must be a ACharacter type to make it swim!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);

		Deactivate();
		return;
	}

	const ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner());

	UOceanBuoyancyComponent* OceanBuoyancyComponent = GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>();
	if (OceanBuoyancyComponent == nullptr)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Owner must have UOceanBuoyancyComponent to make it swim!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);

		Deactivate();
		return;
	}

	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Owner must have UCharacterMovementComponent to make it swim!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);

		Deactivate();
		return;
	}

	OceanBuoyancyComponent->CharacterMovementForceMultiplier = EffectiveSwimmingBuoyancyForce;

	OceanBuoyancyComponent->OnEnteredWater.RemoveDynamic(this, &UOceanSwimmingComponent::OnEnteredWater);
	OceanBuoyancyComponent->OnEnteredWater.AddDynamic(this, &UOceanSwimmingComponent::OnEnteredWater);

	OceanBuoyancyComponent->OnExitedWater.RemoveDynamic(this, &UOceanSwimmingComponent::OnExitedWater);
	OceanBuoyancyComponent->OnExitedWater.AddDynamic(this, &UOceanSwimmingComponent::OnExitedWater);

	OriginalMaxWalkSpeed = CharacterMovement->MaxWalkSpeed;
	OriginalMaxSwimSpeed = CharacterMovement->MaxSwimSpeed;
	OriginalOrientRotationToMovement = CharacterMovement->bOrientRotationToMovement;
	OriginalUseControllerRotationYaw = OwnerAsCharacter->bUseControllerRotationYaw;
	OriginalBuoyancy = CharacterMovement->Buoyancy;
	CharacterMovement->DefaultWaterMovementMode = MOVE_Walking;

	OceanBuoyancyComponent->EnterExitWaterTolerance = (-OwnerAsCharacter->GetCapsuleComponent()->
	                                                                      GetScaledCapsuleHalfHeight())
		+ EnterExitWaterToleranceOffset;

	if (DebugEnabled)
	{
		OwnerAsCharacter->GetCapsuleComponent()->SetHiddenInGame(false);
	}
	else
	{
		OwnerAsCharacter->GetCapsuleComponent()->SetHiddenInGame(true);
	}

	HandleStateAlteration();

	Super::Activate(bReset);
}

void UOceanSwimmingComponent::Deactivate()
{
	Swimming = false;
	HandleStateAlteration();

	StopHeadBubblesEffect();
	StopRightFootBubblesEffect();
	StopLeftFootBubblesEffect();
	StopSwimFastTrailEffect();

	StopDrowningTask();
	StopUnderwaterTask();

	if (IsActive() && HasBegunPlay())
	{
		StopSwimming();
		OnExitedWater("", false);
	}

	if (UOceanBuoyancyComponent* OceanBuoyancyComponent = GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>();
		OceanBuoyancyComponent != nullptr)
	{
		OceanBuoyancyComponent->OnExitedWater.RemoveDynamic(this, &UOceanSwimmingComponent::OnEnteredWater);
		OceanBuoyancyComponent->OnExitedWater.RemoveDynamic(this, &UOceanSwimmingComponent::OnExitedWater);
	}

	OceanologyWater = nullptr;
	OceanologyWaterVolume = nullptr;

	Super::Deactivate();
}

void UOceanSwimmingComponent::Server_SurfaceLockedSwimming_Implementation(const bool Value)
{
	NetMulticast_SurfaceLockedSwimming(Value);
}

void UOceanSwimmingComponent::Server_SwimFast_Implementation(const bool Value)
{
	NetMulticast_SwimFast(Value);
}

void UOceanSwimmingComponent::Server_SwimUpOrDown_Implementation(const bool Value, const double AxisValue)
{
	NetMulticast_SwimUpOrDown(Value, AxisValue);
}

void UOceanSwimmingComponent::Server_LookUp_Implementation(const double AxisValue)
{
	NetMulticast_LookUp(AxisValue);
}

void UOceanSwimmingComponent::Server_MoveForwardBackward_Implementation(const double AxisValue)
{
	NetMulticast_MoveForwardBackward(AxisValue);
}

void UOceanSwimmingComponent::Server_MoveLeftRight_Implementation(const double AxisValue)
{
	NetMulticast_MoveLeftRight(AxisValue);
}


bool UOceanSwimmingComponent::Server_SwimFast_Validate(bool Value)
{
	return GetOwner()->HasAuthority();
}

bool UOceanSwimmingComponent::Server_SurfaceLockedSwimming_Validate(bool Value)
{
	return GetOwner()->HasAuthority();
}

bool UOceanSwimmingComponent::Server_MoveLeftRight_Validate(double AxisValue)
{
	return GetOwner()->HasAuthority();
}

bool UOceanSwimmingComponent::Server_MoveForwardBackward_Validate(double AxisValue)
{
	return GetOwner()->HasAuthority();
}

bool UOceanSwimmingComponent::Server_LookUp_Validate(double AxisValue)
{
	return GetOwner()->HasAuthority();
}

bool UOceanSwimmingComponent::Server_SwimUpOrDown_Validate(const bool Value, const double AxisValue)
{
	return GetOwner()->HasAuthority();
}

void UOceanSwimmingComponent::NetMulticast_SurfaceLockedSwimming_Implementation(const bool Value)
{
	if (!ShouldSwim())
	{
		return;
	}

	SurfaceLockedSwimming = Value;
	SwimUp = false;
	SwimDown = false;
	SwimUpOrDownAxis = 0;

	EffectiveSurfaceLockedSwimming = SurfaceLockedSwimming;

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"SurfaceLockedSwimming: " + UKismetStringLibrary::Conv_BoolToString(SurfaceLockedSwimming),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanSwimmingComponent::NetMulticast_SwimFast_Implementation(const bool Value)
{
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement || !ShouldSwim())
	{
		return;
	}

	SwimFast = Value;
	CharacterMovement->MaxSwimSpeed = Value ? SwimFastMaxSpeed : SwimMaxSpeed;

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Swim Fast: " + UKismetStringLibrary::Conv_BoolToString(SwimFast),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanSwimmingComponent::NetMulticast_SwimUpOrDown_Implementation(const bool Value, const double AxisValue)
{
	if (!ShouldSwim()
		|| SurfaceLockedSwimming
		|| (AxisValue > 0 && GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>()->GetOwnerWaveHeightDistance() >
			SwimUpLimiter))
	{
		SwimUp = false;
		SwimDown = false;
		SwimUpOrDownAxis = 0;
		EffectiveSwimmingBuoyancyForce = 0;
		EffectiveSurfaceLockedSwimming = true;
		return;
	}

	if (EffectiveSwimmingBuoyancyForce == 0)
	{
		EffectiveSwimmingBuoyancyForce = SwimmingBuoyancyForceMultiplier;
		EffectiveSurfaceLockedSwimming = false;
	}

	SwimUp = AxisValue > 0;
	SwimDown = AxisValue < 0;
	SwimUpOrDownAxis = Value ? AxisValue : 0;

	if (Value)
	{
		ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner());
		const FVector ActorForwardVector = AxisValue > 0
			                                   ? OwnerAsCharacter->GetActorForwardVector()
			                                   : -OwnerAsCharacter->GetActorForwardVector();
		const FVector ActorUpVector = OwnerAsCharacter->GetActorUpVector();
		OwnerAsCharacter->AddMovementInput(ActorUpVector + ActorForwardVector, AxisValue);
	}

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"SwimUpOrDown: " + FString::FromInt(AxisValue),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanSwimmingComponent::NetMulticast_LookUp_Implementation(const double AxisValue)
{
	LookUpAxis = AxisValue;
}

void UOceanSwimmingComponent::NetMulticast_MoveForwardBackward_Implementation(const double AxisValue)
{
	MoveForwardBackwardAxis = AxisValue;
}

void UOceanSwimmingComponent::NetMulticast_MoveLeftRight_Implementation(const double AxisValue)
{
	MoveLeftRightAxis = AxisValue;
}

UCharacterMovementComponent* UOceanSwimmingComponent::GetCharacterMovement_Implementation()
{
	return GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
}

bool UOceanSwimmingComponent::CanEnterWater_Implementation(const FName Socket, const bool Underwater)
{
	return true;
}

bool UOceanSwimmingComponent::CanExitWater_Implementation(const FName Socket, const bool Underwater)
{
	return true;
}

bool UOceanSwimmingComponent::CanStartSwim_Implementation()
{
	return true;
}

bool UOceanSwimmingComponent::CanStopSwim_Implementation()
{
	return true;
}

bool UOceanSwimmingComponent::CanAlterState_Implementation()
{
	return true;
}

void UOceanSwimmingComponent::OnEnteredWater(const FName Socket, const bool Underwater)
{
	if (!CanEnterWater(Socket, Underwater) || DrownedToDeath)
	{
		return;
	}

	EnteredWater = true;
	Submerged = Socket.IsEqual(BuoyancyPontoonSocketForEnterWaterEvent) && Underwater;
}

void UOceanSwimmingComponent::OnExitedWater(const FName Socket, const bool Underwater)
{
	if (!CanExitWater(Socket, Underwater))
	{
		return;
	}

	if (!Socket.IsEqual(BuoyancyPontoonSocketForEnterWaterEvent))
	{
		EnteredWater = false;
	}

	Submerged = Socket.IsEqual(BuoyancyPontoonSocketForEnterWaterEvent) && Underwater;
}

void UOceanSwimmingComponent::ComputeWaterFloorHeight()
{
	const UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement)
	{
		return;
	}

	FFindFloorResult OutFloorResult;

	float CapsuleHalfHeight = 100;
	float CapsuleRadius = 50;

	const ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner());
	if (const UCapsuleComponent* CapsuleComponent = OwnerAsCharacter->GetCapsuleComponent())
	{
		CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
		CapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	}

	CharacterMovement->ComputeFloorDist(
		GetOwner()->GetActorLocation(),
		CapsuleHalfHeight,
		CapsuleHalfHeight,
		OutFloorResult,
		CapsuleRadius,
		nullptr
	);

	FindFloorResult = OutFloorResult;
}

void UOceanSwimmingComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OceanologyWater = nullptr;
	OceanologyWaterVolume = nullptr;
}

void UOceanSwimmingComponent::BeginPlay()
{
	Super::BeginPlay();

	OceanologyWater = nullptr;
	OceanologyWaterVolume = nullptr;
}

void UOceanSwimmingComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive()
		|| !OceanologyWater
		|| !EnteredWaterVolume
		|| DrownedToDeath)
	{
		return;
	}

	ComputeWaterFloorHeight();

	if (DebugEnabled)
	{
		const FVector ActorLocation = GetOwner()->GetActorLocation();

		UKismetSystemLibrary::DrawDebugPoint(
			this,
			FVector(ActorLocation.X, ActorLocation.Y,
			        GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>()->GetCurrentWaveHeight().Z),
			100,
			FLinearColor::Green,
			0.0
		);

		UKismetSystemLibrary::PrintString(
			this,
			"Entered Water: " + UKismetStringLibrary::Conv_BoolToString(EnteredWater)
			+ ", Wave Distance: " + FString::FromInt(
				GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>()->GetOwnerWaveHeightDistance())
			+ ", Floor Distance: " + FString::FromInt(FindFloorResult.GetDistanceToFloor())
			+ ", WalkableFloor: " + UKismetStringLibrary::Conv_BoolToString(FindFloorResult.IsWalkableFloor())
			+ ", SurfaceLockedSwimming: " + UKismetStringLibrary::Conv_BoolToString(EffectiveSurfaceLockedSwimming),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	HandleSwimming();
	HandleStateAlteration();
	HandleWaterSurface(DeltaTime);
	HandleUnderwaterTimers();
	HandleUnderwaterEffects();
}

void UOceanSwimmingComponent::HandleSwimming()
{
	if (ShouldSwim())
	{
		StartSwimming();
	}
	else
	{
		StopSwimming();
	}
}

void UOceanSwimmingComponent::StartSwimming()
{
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement || CharacterMovement->MovementMode == MOVE_Swimming || !CanStartSwim())
	{
		return;
	}

	CharacterMovement->SetMovementMode(MOVE_Swimming);
	Swimming = true;
	LastMovementMode = CharacterMovement->MovementMode;
	OnStartSwimming.Broadcast();

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Movement Mode Changed: Started Swimming",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanSwimmingComponent::StopSwimming()
{
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement || CharacterMovement->MovementMode != MOVE_Swimming || !CanStopSwim())
	{
		return;
	}

	CharacterMovement->SetMovementMode(FindFloorResult.IsWalkableFloor() ? MOVE_Walking : MOVE_Falling);
	Swimming = false;
	LastMovementMode = CharacterMovement->MovementMode;
	OnStopSwimming.Broadcast();

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Movement Mode Changed: Stopped Swimming",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

bool UOceanSwimmingComponent::ShouldSwim() const
{
	return !DrownedToDeath
		&& EnteredWaterVolume
		&& EnteredWater
		&& GetOwner()->GetPhysicsVolume()->bWaterVolume
		&& GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>()->GetOwnerWaveHeightDistance() < Immersion
		&& (!FindFloorResult.IsWalkableFloor() || Submerged);
}

void UOceanSwimmingComponent::HandleStateAlteration()
{
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement || !CanAlterState())
	{
		return;
	}

	ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner());

	if (Swimming)
	{
		OwnerAsCharacter->bUseControllerRotationYaw = SwimmingUseControllerRotationYaw;
		CharacterMovement->MaxWalkSpeed = OceanologyWater->GetWaterDensity(GetOwner());
		CharacterMovement->bOrientRotationToMovement = SwimmingOrientRotationToMovement;
		CharacterMovement->Buoyancy = 1;
		CharacterMovement->MaxSwimSpeed = SwimMaxSpeed;
	}
	else
	{
		OwnerAsCharacter->bUseControllerRotationYaw = OriginalUseControllerRotationYaw;
		CharacterMovement->MaxWalkSpeed = OriginalMaxWalkSpeed;
		CharacterMovement->bOrientRotationToMovement = OriginalOrientRotationToMovement;
		CharacterMovement->Buoyancy = OriginalBuoyancy;
		CharacterMovement->MaxSwimSpeed = OriginalMaxSwimSpeed;
	}
}

void UOceanSwimmingComponent::SetupUnderwaterEffects()
{
	const UOceanologyRuntimeSettings* OceanologyRuntimeSettings = GetDefault<UOceanologyRuntimeSettings>();

	if (!HeadBubblesEffectNiagara)
	{
		HeadBubblesEffectNiagara = OceanologyRuntimeSettings->GetHeadBubblesEffectNiagara();
	}

	if (!RightFootBubblesEffectNiagara)
	{
		RightFootBubblesEffectNiagara = OceanologyRuntimeSettings->GetRightFootBubblesEffectNiagara();
	}

	if (!LeftFootBubblesEffectNiagara)
	{
		LeftFootBubblesEffectNiagara = OceanologyRuntimeSettings->GetLeftFootBubblesEffectNiagara();
	}

	if (!SwimFastTrailEffectNiagara)
	{
		SwimFastTrailEffectNiagara = OceanologyRuntimeSettings->GetSwimFastTrailEffectNiagara();
	}

	if (!SwimFastTrailSound)
	{
		SwimFastTrailSound = OceanologyRuntimeSettings->GetSwimFastTrailSound();
	}
}

void UOceanSwimmingComponent::HandleWaterSurface(const float DeltaTime)
{
	const UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	if (!CharacterMovement)
	{
		return;
	}

	if (!ShouldSwim())
	{
		EffectiveSurfaceLockedSwimming = SurfaceLockedSwimming;
		EffectiveSwimmingBuoyancyForce = SwimmingBuoyancyForceMultiplier;
		return;
	}

	if (UKismetMathLibrary::VSizeSquared(CharacterMovement->Velocity) < 100.0
		&& !FindFloorResult.IsWalkableFloor()
		&& FindFloorResult.GetDistanceToFloor() >= 10
		&& !Submerged)
	{
		EffectiveSurfaceLockedSwimming = true;
	}
	else if (FindFloorResult.IsWalkableFloor() || FindFloorResult.GetDistanceToFloor() < 10)
	{
		EffectiveSurfaceLockedSwimming = false;
		return;
	}

	const double WaveHeightZ = GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>()->GetCurrentWaveHeight().Z;
	if (const FVector ActorLocation = GetOwner()->GetActorLocation();
		GetOwner()->GetComponentByClass<UOceanBuoyancyComponent>()->GetOwnerWaveHeightDistance() >
		SurfaceLockedSwimmingLimiter && !SwimUp && !SwimDown)
	{
		if (EffectiveSurfaceLockedSwimming)
		{
			EffectiveSwimmingBuoyancyForce = 0;

			GetOwner()->SetActorLocation(UKismetMathLibrary::VInterpTo(
				ActorLocation,
				FVector(ActorLocation.X, ActorLocation.Y, WaveHeightZ),
				DeltaTime,
				0
			));
		}
	}
	else
	{
		EffectiveSwimmingBuoyancyForce = SwimmingBuoyancyForceMultiplier;
	}
}

void UOceanSwimmingComponent::HandleUnderwaterEffects()
{
	if (UKismetSystemLibrary::IsDedicatedServer(this)
		|| !EnableUnderwaterEffects
		|| !OceanologyWater
		|| !OceanologyWater->UnderwaterComponent
		|| !OceanologyWater->UnderwaterComponent->BubblesSettings.BubblesMaterial)
	{
		return;
	}

	if (!SwimmingBubblesMID)
	{
		SwimmingBubblesMID = FOceanologyWaterUtils::GetOrCreateTransientMID(
			SwimmingBubblesMID,
			TEXT("SwimmingBubblesMID"),
			OceanologyWater->UnderwaterComponent->BubblesSettings.BubblesMaterial.LoadSynchronous(),
			FOceanologyWaterUtils::GetTransientMIDFlags()
		);
	}

	if (!Submerged)
	{
		StopHeadBubblesEffect();
		StopRightFootBubblesEffect();
		StopLeftFootBubblesEffect();
		StopSwimFastTrailEffect();
		return;
	}

	const ACharacter* OwnerAsCharacter = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* MeshComponent = OwnerAsCharacter->GetMesh();
	if (!MeshComponent)
	{
		return;
	}

	StartHeadBubblesEffect(MeshComponent);
	StartRightFootBubblesEffect(MeshComponent);
	StartLeftFootBubblesEffect(MeshComponent);

	if (SwimFast)
	{
		if (StopHeadBubblesWhileSwimFast)
		{
			StopHeadBubblesEffect();
		}

		if (StopRightFootBubblesWhileSwimFast)
		{
			StopRightFootBubblesEffect();
		}

		if (StopLeftFootBubblesWhileSwimFast)
		{
			StopLeftFootBubblesEffect();
		}

		StartSwimFastTrailEffect(MeshComponent);
	}
	else
	{
		StopSwimFastTrailEffect();
	}
}

void UOceanSwimmingComponent::StartHeadBubblesEffect(USkeletalMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr || HeadBubblesEffectNiagara == nullptr || HeadBubblesEffect != nullptr)
	{
		return;
	}

	if (SwimFast && StopHeadBubblesWhileSwimFast)
	{
		return;
	}

	HeadBubblesEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		HeadBubblesEffectNiagara,
		MeshComponent,
		HeadSocketName,
		FVector(),
		FRotator(0, 0, 0),
		EAttachLocation::SnapToTarget,
		true,
		true
	);
	HeadBubblesEffect->SetVariableMaterial(
		"DynamicMaterial",
		SwimmingBubblesMID
	);
}

void UOceanSwimmingComponent::StartRightFootBubblesEffect(USkeletalMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr || RightFootBubblesEffectNiagara == nullptr || RightFootBubblesEffect != nullptr)
	{
		return;
	}

	if (SwimFast && StopRightFootBubblesWhileSwimFast)
	{
		return;
	}

	RightFootBubblesEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		RightFootBubblesEffectNiagara,
		MeshComponent,
		RightFootSocketName,
		FVector(),
		FRotator(0, 0, 0),
		EAttachLocation::SnapToTarget,
		true,
		true
	);
	RightFootBubblesEffect->SetVariableMaterial(
		"DynamicMaterial",
		SwimmingBubblesMID
	);
}

void UOceanSwimmingComponent::StartLeftFootBubblesEffect(USkeletalMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr || LeftFootBubblesEffectNiagara == nullptr || LeftFootBubblesEffect != nullptr)
	{
		return;
	}

	if (SwimFast && StopLeftFootBubblesWhileSwimFast)
	{
		return;
	}

	LeftFootBubblesEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		LeftFootBubblesEffectNiagara,
		MeshComponent,
		LeftFootSocketName,
		FVector(),
		FRotator(0, 0, 0),
		EAttachLocation::SnapToTarget,
		true,
		true
	);
	LeftFootBubblesEffect->SetVariableMaterial(
		"DynamicMaterial",
		SwimmingBubblesMID
	);
}

void UOceanSwimmingComponent::StartSwimFastTrailEffect(USkeletalMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr || SwimFastTrailEffectNiagara == nullptr || SwimFastTrailEffect != nullptr)
	{
		return;
	}

	SwimFastTrailEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		SwimFastTrailEffectNiagara,
		MeshComponent,
		SwimFastTrailSocketName,
		FVector(0, 0, 0),
		FRotator(0, 0, 0),
		EAttachLocation::SnapToTarget,
		true,
		true
	);
	SwimFastTrailEffect->SetVariableMaterial(
		"DynamicMaterial",
		SwimmingBubblesMID
	);

	if (SwimFastTrailSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SwimFastTrailSound,
			GetOwner()->GetActorLocation()
		);
	}
}

void UOceanSwimmingComponent::StopHeadBubblesEffect()
{
	if (HeadBubblesEffect != nullptr)
	{
		HeadBubblesEffect->DestroyInstance();
		HeadBubblesEffect = nullptr;
	}
}

void UOceanSwimmingComponent::StopRightFootBubblesEffect()
{
	if (RightFootBubblesEffect != nullptr)
	{
		RightFootBubblesEffect->DestroyInstance();
		RightFootBubblesEffect = nullptr;
	}
}

void UOceanSwimmingComponent::StopLeftFootBubblesEffect()
{
	if (LeftFootBubblesEffect != nullptr)
	{
		LeftFootBubblesEffect->DestroyInstance();
		LeftFootBubblesEffect = nullptr;
	}
}

void UOceanSwimmingComponent::StopSwimFastTrailEffect()
{
	if (SwimFastTrailEffect != nullptr)
	{
		SwimFastTrailEffect->DestroyInstance();
		SwimFastTrailEffect = nullptr;
	}
}

void UOceanSwimmingComponent::HandleUnderwaterTimers()
{
	if (DrownedToDeath)
	{
		StopSwimming();
		OnExitedWater("", false);
		return;
	}

	if (!Submerged)
	{
		StopDrowningTask();
		StopUnderwaterTask();
		return;
	}

	StartUnderwaterTask();

	if (UnderwaterTimeCounter > DrowningTimeWarningInSeconds)
	{
		StartDrowningTask();

		if (DrowningTimeCounter > DrowningTimeDeathInSeconds)
		{
			DrownToDeath();
		}
	}
	else
	{
		StopDrowningTask();
	}
}

void UOceanSwimmingComponent::StartUnderwaterTask()
{
	if (UnderwaterTaskRunning)
	{
		return;
	}

	UnderwaterTaskRunning = true;
	UnderwaterTimeCounter = 0;

	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
		this,
		&UOceanSwimmingComponent::UnderwaterTask
	);

	GetOwner()->GetWorldTimerManager().SetTimer(
		UnderwaterTaskDelegate,
		TimerDelegate,
		1.0f,
		true
	);

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Started Underwater task.",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	OnUnderwaterEnter.Broadcast();
}

void UOceanSwimmingComponent::StopUnderwaterTask()
{
	if (!UnderwaterTaskRunning)
	{
		return;
	}

	UnderwaterTaskRunning = false;
	UnderwaterTimeCounter = 0;

	GetOwner()->GetWorldTimerManager().ClearTimer(UnderwaterTaskDelegate);

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Stopped Underwater task.",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	OnUnderwaterExit.Broadcast();
}

void UOceanSwimmingComponent::StartDrowningTask()
{
	if (!DrowningEnabled || DrowningTaskRunning)
	{
		return;
	}

	DrowningTaskRunning = true;
	DrowningTimeCounter = 0;

	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
		this,
		&UOceanSwimmingComponent::DrowningTask
	);

	GetOwner()->GetWorldTimerManager().SetTimer(
		DrowningTaskDelegate,
		TimerDelegate,
		1.0f,
		true
	);

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Started Drowning task.",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	OnStartDrowning.Broadcast();
}

void UOceanSwimmingComponent::StopDrowningTask()
{
	if (!DrowningEnabled || !DrowningTaskRunning)
	{
		return;
	}

	DrowningTaskRunning = false;
	DrowningTimeCounter = 0;

	GetOwner()->GetWorldTimerManager().ClearTimer(DrowningTaskDelegate);

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Stopped Drowning task.",
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}

	OnStopDrowning.Broadcast();
}


void UOceanSwimmingComponent::UnderwaterTask()
{
	OnUnderwaterTask.Broadcast(UnderwaterTimeCounter);
	UnderwaterTimeCounter++;

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"UnderwaterTimeCounter: " + FString::FromInt(UnderwaterTimeCounter),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanSwimmingComponent::DrowningTask()
{
	if (!DrowningEnabled)
	{
		return;
	}

	OnDrowningTask.Broadcast(DrowningTimeCounter);
	DrowningTimeCounter++;

	if (DebugEnabled)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"DrowningTimeCounter: " + FString::FromInt(DrowningTimeCounter),
			true,
			true,
			FLinearColor::Green,
			1.0
		);
	}
}

void UOceanSwimmingComponent::DrownToDeath()
{
	if (!DrowningEnabled)
	{
		return;
	}

	DrownedToDeath = true;

	StopSwimming();
	OnExitedWater("", false);
	Submerged = false;

	StopDrowningTask();
	StopUnderwaterTask();

	OnDrownDeath.Broadcast();
}

void UOceanSwimmingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOceanSwimmingComponent, OceanologyWater);
	DOREPLIFETIME(UOceanSwimmingComponent, OceanologyWaterVolume);

	DOREPLIFETIME(UOceanSwimmingComponent, SurfaceLockedSwimming);
	DOREPLIFETIME(UOceanSwimmingComponent, SwimDown);
	DOREPLIFETIME(UOceanSwimmingComponent, SwimUp);
	DOREPLIFETIME(UOceanSwimmingComponent, SwimFast);
	DOREPLIFETIME(UOceanSwimmingComponent, SwimUpOrDownAxis);
	DOREPLIFETIME(UOceanSwimmingComponent, LookUpAxis);
	DOREPLIFETIME(UOceanSwimmingComponent, MoveForwardBackwardAxis);
	DOREPLIFETIME(UOceanSwimmingComponent, MoveLeftRightAxis);

	DOREPLIFETIME(UOceanSwimmingComponent, EnteredWaterVolume);
	DOREPLIFETIME(UOceanSwimmingComponent, EnteredWater);
	DOREPLIFETIME(UOceanSwimmingComponent, Submerged);
	DOREPLIFETIME(UOceanSwimmingComponent, Swimming);
	DOREPLIFETIME(UOceanSwimmingComponent, FindFloorResult);
	DOREPLIFETIME(UOceanSwimmingComponent, EffectiveSwimmingBuoyancyForce);
	DOREPLIFETIME(UOceanSwimmingComponent, EffectiveSurfaceLockedSwimming);
	DOREPLIFETIME(UOceanSwimmingComponent, LastMovementMode);

	DOREPLIFETIME(UOceanSwimmingComponent, OriginalMaxWalkSpeed);
	DOREPLIFETIME(UOceanSwimmingComponent, OriginalMaxSwimSpeed);
	DOREPLIFETIME(UOceanSwimmingComponent, OriginalOrientRotationToMovement);
	DOREPLIFETIME(UOceanSwimmingComponent, OriginalUseControllerRotationYaw);
	DOREPLIFETIME(UOceanSwimmingComponent, OriginalBuoyancy);

	DOREPLIFETIME(UOceanSwimmingComponent, UnderwaterTimeCounter);
	DOREPLIFETIME(UOceanSwimmingComponent, UnderwaterTaskRunning);

	DOREPLIFETIME(UOceanSwimmingComponent, DrowningTimeCounter);
	DOREPLIFETIME(UOceanSwimmingComponent, DrowningTaskRunning);
	DOREPLIFETIME(UOceanSwimmingComponent, DrownedToDeath);
}
