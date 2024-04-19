// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Components/OceanBuoyancyComponent.h"
#include "DrawDebugHelpers.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/Wave/OceanologyWaveSolverComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsSettings.h"

UOceanBuoyancyComponent::UOceanBuoyancyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
	bTickInEditor = false;
}

void UOceanBuoyancyComponent::Activate(const bool bReset)
{
	SetComponentTickInterval(BuoyancyUpdateInterval);

	OwnerMesh = GetOwnerMesh();
	CharacterMovement = GetCharacterMovement();

	RadiusWithGravitySigned = FMath::Sign(GetGravityZ());

	if (IsValid(OwnerMesh))
	{
		OwnerMeshLinearDamping = OwnerMesh->GetLinearDamping();
		OwnerMeshAngularDamping = OwnerMesh->GetAngularDamping();
	}

	if (OceanologyWater)
	{
		const double GlobalWaveDirection = OceanologyWater->GetWaveSolver()->GetGlobalWaveDirection();
		GlobalWaveDirectionX = FMath::Sin(GlobalWaveDirection);
		GlobalWaveDirectionY = FMath::Cos(GlobalWaveDirection);
	}

	FirstInit = true;

	Super::Activate(bReset);
}

void UOceanBuoyancyComponent::Deactivate()
{
	OceanologyWater = nullptr;
	OceanologyWaterVolume = nullptr;

	Super::Deactivate();
}

void UOceanBuoyancyComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OceanologyWater = nullptr;
	OceanologyWaterVolume = nullptr;
}

void UOceanBuoyancyComponent::BeginPlay()
{
	Super::BeginPlay();

	OceanologyWater = nullptr;
	OceanologyWaterVolume = nullptr;
}

void UOceanBuoyancyComponent::DebugPontoons()
{
	OwnerMesh = GetOwnerMesh();

	if (OwnerMesh == nullptr)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Owner Mesh is missing! Cannot debug pontoons!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);
		return;
	}

	if (Pontoons.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(
			this,
			"You do not have pontoons to debug!",
			true,
			true,
			FLinearColor::Yellow,
			1.0
		);
		return;
	}

	for (auto [Socket, Pontoon, Radius, DensityOverride, Mode] : Pontoons)
	{
		const FVector Position = OwnerMesh->GetComponentTransform().TransformPosition(Pontoon);

		UKismetSystemLibrary::DrawDebugSphere(
			this,
			Position,
			Radius * DebugSphereRadiusMultiplier,
			12,
			DebugSphereColorPontoons,
			3,
			1.0
		);
	}
}

FVector UOceanBuoyancyComponent::GetCurrentWaveHeight() const
{
	return CurrentWaveHeight;
}

double UOceanBuoyancyComponent::GetOwnerWaveHeightDistance() const
{
	return GetOwner()->GetActorLocation().Z - (CurrentWaveHeight.Z + EnterExitWaterTolerance);
}

UCharacterMovementComponent* UOceanBuoyancyComponent::GetCharacterMovement_Implementation()
{
	return GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
}

void UOceanBuoyancyComponent::SetFlowControlSpline(
	USplineComponent* InFlowControlSpline,
	const double InUnscaledSplineWidth
)
{
	FlowControlSpline = InFlowControlSpline;
	UnscaledSplineWidth = InUnscaledSplineWidth;
}

USplineComponent* UOceanBuoyancyComponent::GetFlowControlSpline() const
{
	return FlowControlSpline;
}

void UOceanBuoyancyComponent::TickComponent(
	const float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	if (!OwnerMesh || FirstInit)
	{
		OwnerMesh = GetOwnerMesh();
		CharacterMovement = GetCharacterMovement();
		FirstInit = false;
	}

	if (!IsActive()
		|| !OceanologyWater
		|| !EnteredWaterVolume
		|| !OwnerMesh)
	{
		return;
	}

	ApplyBuoyancyToMesh();
	ApplyFlowToMesh();
}

void UOceanBuoyancyComponent::AddForceAtLocation(
	const FVector& Force,
	const FVector& Position
) const
{
	if (!OwnerMesh)
	{
		return;
	}

	if (OwnerMesh->IsSimulatingPhysics(""))
	{
		OwnerMesh->AddForceAtLocation(Force, Position);
	}
	else if (CharacterMovement)
	{
		CharacterMovement->AddForce(Force * CharacterMovementForceMultiplier);
	}
}

FVector UOceanBuoyancyComponent::GetVelocityAtLocation(
	const FVector& Point
) const
{
	if (!OwnerMesh)
	{
		return FVector::ZeroVector;
	}

	if (const FBodyInstance* BodyInstance = OwnerMesh->GetBodyInstance("");
		BodyInstance != nullptr
		&& BodyInstance->IsValidBodyInstance())
	{
		return BodyInstance->GetUnrealWorldVelocityAtPoint(Point);
	}

	return FVector::ZeroVector;
}

UPrimitiveComponent* UOceanBuoyancyComponent::GetOwnerMesh_Implementation() const
{
	UPrimitiveComponent* PrimitiveComponent = GetOwner()->GetComponentByClass<UPrimitiveComponent>();
	USkeletalMeshComponent* SkeletalMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	return PrimitiveComponent->IsSimulatingPhysics("") ? PrimitiveComponent : SkeletalMeshComponent;
}

void UOceanBuoyancyComponent::ApplyBuoyancyToMesh()
{
	if (!OwnerMesh)
	{
		return;
	}

	const double TotalPontoons = Pontoons.Num();
	if (TotalPontoons < 1)
	{
		return;
	}

	const FTransform Transform = OwnerMesh->GetComponentToWorld();

	int PontoonsUnderwater = 0;
	for (int PontoonIndex = 0; PontoonIndex < TotalPontoons; PontoonIndex++)
	{
		if (!Pontoons.IsValidIndex(PontoonIndex))
		{
			continue;
		}

		auto [Socket, Pontoon, Radius, DensityOverride, Mode] = Pontoons[PontoonIndex];

		bool Underwater = false;
		const FVector Position = !Socket.IsNone()
			                         ? OwnerMesh->GetSocketTransform(Socket, RTS_World).TransformPosition(Pontoon)
			                         : Transform.TransformPosition(Pontoon);

		CurrentWaveHeight = OceanologyWater->GetWaveHeightAtLocation(Position);
		const double WaveHeightAtLocationZ = CurrentWaveHeight.Z;
		const double LocalRadiusWithGravitySigned = RadiusWithGravitySigned * FMath::Abs(Radius);

		if (WaveHeightAtLocationZ > (Position.Z + LocalRadiusWithGravitySigned))
		{
			PontoonsUnderwater++;
			Underwater = true;

			double LocalMultiplier = (WaveHeightAtLocationZ - (Position.Z + LocalRadiusWithGravitySigned))
				/ (Radius * 2);
			LocalMultiplier = FMath::Clamp(LocalMultiplier, 0.f, 1.f);

			const double PontoonDensity = DensityOverride > 0 ? DensityOverride : DefaultMeshDensity;
			const float Mass = GetMass();
			const double AccumulatedBuoyancyForceZ = Mass
				/ PontoonDensity
				* WaterFluidDensity
				* -GetGravityZ()
				/ TotalPontoons
				* LocalMultiplier;

			FVector AccumulatedDampingForce = -GetVelocityAtLocation(Position)
				* WaterVelocityDamper
				* Mass
				* LocalMultiplier;

			if (GlobalWaveForceEnabled)
			{
				const double WaveVelocityAtLocation = FMath::Clamp(
					GetVelocityAtLocation(Position).Z,
					-20.f,
					150.f
				) * (1 - LocalMultiplier);

				AccumulatedDampingForce += FVector(
						GlobalWaveDirectionX,
						GlobalWaveDirectionY,
						0
					) * Mass
					* WaveVelocityAtLocation
					* GlobalWaveForceMultiplier / TotalPontoons;
			}

			if (Mode == Buoyancy)
			{
				AddForceAtLocation(
					FVector(
						AccumulatedDampingForce.X,
						AccumulatedDampingForce.Y,
						AccumulatedDampingForce.Z + AccumulatedBuoyancyForceZ
					),
					Position
				);
			}
		}


		if (WaveHeightAtLocationZ > Position.Z + EnterExitWaterTolerance)
		{
			OnEnteredWater.Broadcast(Socket, Underwater);
		}
		else
		{
			OnExitedWater.Broadcast(Socket, Underwater);
		}

		if (DebugEnabled)
		{
			UKismetSystemLibrary::DrawDebugSphere(
				this,
				Position,
				Radius * DebugSphereRadiusMultiplier,
				12,
				Underwater ? DebugSphereColorBuoyancyLocationUnderwater : DebugSphereColorBuoyancyLocation,
				0,
				1.0
			);
		}
	}

	if (LimitUnderwaterMaxVelocity
		&& PontoonsUnderwater > 0
		&& OwnerMesh->GetPhysicsLinearVelocity().Size() > MaxUnderwaterVelocity)
	{
		const FVector LastVelocity = OwnerMesh->GetPhysicsLinearVelocity().GetSafeNormal()
			* MaxUnderwaterVelocity;
		OwnerMesh->SetPhysicsLinearVelocity(LastVelocity);
	}

	OwnerMesh->SetLinearDamping(
		OwnerMeshLinearDamping + WaterFluidLinearDamping / TotalPontoons * PontoonsUnderwater
	);

	OwnerMesh->SetAngularDamping(
		OwnerMeshAngularDamping + WaterFluidAngularDamping / TotalPontoons * PontoonsUnderwater
	);
}

void UOceanBuoyancyComponent::ApplyFlowToMesh()
{
	if (!OwnerMesh
		|| !FlowControlSpline
		|| FlowControlSpline->GetNumberOfSplinePoints() < 1)
	{
		return;
	}

	const double CurrentInputKey = FlowControlSpline->FindInputKeyClosestToWorldLocation(
		OwnerMesh->GetComponentLocation()
	);

	if (DebugEnabled)
	{
		// Progress on the flow control spline.
		UKismetSystemLibrary::DrawDebugString(
			this,
			FVector(0, 0, 300.0),
			"Progress: "
			+ FString::SanitizeFloat(CurrentInputKey)
			+ " / "
			+ FString::FromInt(FlowControlSpline->GetNumberOfSplinePoints() - 1),
			OwnerMesh->GetOwner(),
			FLinearColor::Green,
			0
		);
	}

	if (CurrentInputKey == (FlowControlSpline->GetNumberOfSplinePoints() - 1))
	{
		SetFlowControlSpline(nullptr, 0);
		return;
	}

	const FVector CurrentLocation = FlowControlSpline->GetLocationAtSplineInputKey(
		CurrentInputKey,
		ESplineCoordinateSpace::World
	);
	const FVector CurrentDirection = FlowControlSpline->GetDirectionAtSplinePoint(
		CurrentInputKey,
		ESplineCoordinateSpace::World
	);
	const FVector CurrentRightVectorAtSplineInputKey = FlowControlSpline->GetRightVectorAtSplineInputKey(
		CurrentInputKey,
		ESplineCoordinateSpace::World
	);
	const double CurrentSplineWidth = FlowControlSpline->GetScaleAtSplineInputKey(CurrentInputKey).Y *
		UnscaledSplineWidth;

	// NormalizedWidthOffset. (NOT clamped between -1, 1)
	const double NormalizedWidthOffset = FVector::DotProduct(OwnerMesh->GetComponentLocation() - CurrentLocation,
	                                                         CurrentRightVectorAtSplineInputKey)
		/ CurrentSplineWidth;

	// Current point on spline.
	const FVector CurrentPointOnSpline = ((CurrentRightVectorAtSplineInputKey * NormalizedWidthOffset)
			* CurrentSplineWidth)
		+ CurrentLocation;

	if (DebugEnabled)
	{
		// WidthScaleBox.
		UKismetSystemLibrary::DrawDebugBox(
			this,
			CurrentLocation + FVector(0, 0, 100.0),
			FVector(CurrentRightVectorAtSplineInputKey * UnscaledSplineWidth),
			FLinearColor::Green,
			FlowControlSpline->GetRotationAtSplineInputKey(CurrentInputKey, ESplineCoordinateSpace::World),
			0,
			5.0
		);

		// Width Offset.
		UKismetSystemLibrary::DrawDebugString(
			this,
			FVector(0, 0, 100.0),
			"Width Offset: " + FString::SanitizeFloat(NormalizedWidthOffset * -1.0),
			OwnerMesh->GetOwner(),
			FLinearColor::Green,
			0
		);
	}

	// Upcoming point on spline.
	const double UpcomingInputKey = CurrentInputKey + 0.05; // Arbitrary small number to query further along on spline.
	const FVector UpcomingRightVectorAtSplineInputKey = FlowControlSpline->GetRightVectorAtSplineInputKey(
		UpcomingInputKey,
		ESplineCoordinateSpace::World
	);
	const double UpcomingSplineWidth = FlowControlSpline->GetScaleAtSplineInputKey(UpcomingInputKey).Y *
		UnscaledSplineWidth;
	const FVector UpcomingLocation = FlowControlSpline->GetLocationAtSplineInputKey(
		UpcomingInputKey,
		ESplineCoordinateSpace::World
	);
	const FVector UpcomingPointOnSpline = UpcomingLocation + ((UpcomingRightVectorAtSplineInputKey *
		NormalizedWidthOffset) * UpcomingSplineWidth);

	const double Angle = UKismetMathLibrary::DegAcos(FVector::DotProduct(
		UKismetMathLibrary::Vector_Normal2D(UpcomingPointOnSpline - CurrentPointOnSpline, 0.0001),
		UKismetMathLibrary::Vector_Normal2D(CurrentDirection, 0.0001)
	)) * (NormalizedWidthOffset * -1.0);

	if (DebugEnabled)
	{
		// Angle.
		UKismetSystemLibrary::DrawDebugString(
			this,
			FVector(0, 0, 50.0),
			"Angle: " + FString::SanitizeFloat(Angle),
			OwnerMesh->GetOwner(),
			FLinearColor::Red,
			0
		);

		// Purple Arrow: Spline Edge Direction.
		const FVector SplineEdgeDirectionLineStart = CurrentPointOnSpline + FVector(0, 0, 100.0);
		const FVector SplineEdgeDirectionLineEnd = UpcomingPointOnSpline + FVector(0, 0, 100.0);

		UKismetSystemLibrary::DrawDebugArrow(
			this,
			SplineEdgeDirectionLineStart,
			SplineEdgeDirectionLineEnd,
			25.0,
			FLinearColor(0.126276, 0, 1.0), // Dark Purple
			0.1,
			4.0
		);
	}

	// Angle-adjusted force.
	const FVector AngleAdjustedForce = UKismetMathLibrary::RotateAngleAxis(
		CurrentDirection * 100.0,
		Angle,
		FVector::ZAxisVector
	);

	if (DebugEnabled)
	{
		const FVector AngleAdjustedForceLineStart = OwnerMesh->GetComponentLocation() + FVector(0, 0, 100.0);
		const FVector AngleAdjustedForceLineEnd = AngleAdjustedForceLineStart + AngleAdjustedForce;

		UKismetSystemLibrary::DrawDebugArrow(
			this,
			AngleAdjustedForceLineStart,
			AngleAdjustedForceLineEnd,
			25.0,
			FLinearColor(0.43572, 0.354167, 1.0), // Light Purple
			0.1,
			4.0
		);
	}

	OwnerMesh->AddForce(AngleAdjustedForce * AngleAdjustedForceStrength);

	if (OrientMeshRotationYawTowardsFlowDirection)
	{
		const FRotator ComponentRotation = OwnerMesh->GetComponentRotation();
		const FRotator SplinePointRotation = FlowControlSpline->GetRotationAtSplineInputKey(
			CurrentInputKey,
			ESplineCoordinateSpace::World
		);
		const FRotator TargetRotation(
			ComponentRotation.Pitch,
			SplinePointRotation.Yaw,
			ComponentRotation.Roll
		);

		OwnerMesh->SetWorldRotation(TargetRotation);
	}
}

float UOceanBuoyancyComponent::GetMass() const
{
	if (!OwnerMesh)
	{
		return 0;
	}

	if (!OwnerMesh->IsSimulatingPhysics("") && CharacterMovement)
	{
		return CharacterMovement->Mass;
	}

	return OwnerMesh->GetMass();
}

float UOceanBuoyancyComponent::GetGravityZ() const
{
	if (CharacterMovement)
	{
		return CharacterMovement->GetGravityZ();
	}

	return UPhysicsSettings::Get()->DefaultGravityZ;
}

void UOceanBuoyancyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOceanBuoyancyComponent, OceanologyWater);
	DOREPLIFETIME(UOceanBuoyancyComponent, OceanologyWaterVolume);
	DOREPLIFETIME(UOceanBuoyancyComponent, EnteredWaterVolume);
	DOREPLIFETIME(UOceanBuoyancyComponent, CurrentWaveHeight);
}
