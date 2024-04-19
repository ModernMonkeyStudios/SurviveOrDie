// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanBuoyancyComponent.generated.h"

class USplineComponent;
class AOceanologyWaterVolume;

UENUM(BlueprintType)
enum EOceanBuoyancyPontoonMode
{
	Buoyancy,
	WaterEnterExitEventOnly
};

USTRUCT(BlueprintType)
struct OCEANOLOGY_PLUGIN_API FOceanBuoyancyPontoon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pontoon")
	FName Socket = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pontoon")
	FVector Pontoon = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pontoon")
	double Radius = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pontoon")
	double DensityOverride = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pontoon")
	TEnumAsByte<EOceanBuoyancyPontoonMode> Mode = Buoyancy;

	FOceanBuoyancyPontoon()
	{
	}

	FOceanBuoyancyPontoon(
		const FName& InSocket,
		const FVector& InPontoon,
		const double& InRadius,
		const double& InDensityOverride,
		const TEnumAsByte<EOceanBuoyancyPontoonMode>& InMode
	)
	{
		Socket = InSocket;
		Pontoon = InPontoon;
		Radius = InRadius;
		DensityOverride = InDensityOverride;
		Mode = InMode;
	}
};

class UPrimitiveComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UCharacterMovementComponent;
class AOceanologyWaterParent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOceanOnEnteredWater, FName, Socket, bool, Underwater);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOceanOnExitedWater, FName, Socket, bool, Underwater);

UCLASS(BlueprintType, Blueprintable, ClassGroup = Buoyancy, meta = (BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanBuoyancyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanBuoyancyComponent();

	/** Value 0 means real-time buoyancy. Increasing the value provides better performance, but buoyancy might become more inaccurate. Be careful changing this. This controls tick rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	float BuoyancyUpdateInterval = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	double DefaultMeshDensity = 700.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	double WaterFluidDensity = 1030.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	double WaterFluidLinearDamping = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	double WaterFluidAngularDamping = 2.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	FVector WaterVelocityDamper = FVector(0.1, 0.1, 0.1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	bool LimitUnderwaterMaxVelocity = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	double MaxUnderwaterVelocity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	TArray<FOceanBuoyancyPontoon> Pontoons = {FOceanBuoyancyPontoon()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	double GlobalWaveForceMultiplier = 2.0f;

	/** Enabling this will make global wave forces of the ocean affect the buoyancy direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	bool GlobalWaveForceEnabled = false;

	/** This defines the force strength of the flow physics based on angle. Turn on debug to see the angles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy|Flow")
	double AngleAdjustedForceStrength = 35.0;

	/** Orient the mesh rotation towards the direction of the flow if a flow is present. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy|Flow")
	bool OrientMeshRotationYawTowardsFlowDirection = false;

	/** Enable/disable special debug messages and spheres that indicate buoyancy interactions. WARNING: Debugging affects performance, do NOT use in production! */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Buoyancy|Debug")
	bool DebugEnabled = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Buoyancy|Debug")
	FLinearColor DebugSphereColorPontoons = FLinearColor(1, 1, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Buoyancy|Debug")
	FLinearColor DebugSphereColorBuoyancyLocation = FLinearColor(1, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Buoyancy|Debug")
	FLinearColor DebugSphereColorBuoyancyLocationUnderwater = FLinearColor(1, 0, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Buoyancy|Debug")
	double DebugSphereRadiusMultiplier = 10.0;

	UPROPERTY(BlueprintReadOnly, Category="Buoyancy", Replicated)
	bool EnteredWaterVolume = false;

	virtual void Activate(const bool bReset) override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Buoyancy")
	void ToggleDebug() { DebugEnabled = !DebugEnabled; }

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Buoyancy")
	void DebugPontoons();

	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
	FVector GetCurrentWaveHeight() const;

	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
	double GetOwnerWaveHeightDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
	void AddForceAtLocation(
		const FVector& Force,
		const FVector& Position
	) const;

	UFUNCTION(BlueprintPure, Category = "Buoyancy")
	FVector GetVelocityAtLocation(
		const FVector& Point
	) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Buoyancy")
	UPrimitiveComponent* GetOwnerMesh() const;

	UPROPERTY(BlueprintAssignable, Category="Buoyancy")
	FOceanOnEnteredWater OnEnteredWater;

	UPROPERTY(BlueprintAssignable, Category="Buoyancy")
	FOceanOnExitedWater OnExitedWater;

	UPROPERTY()
	double CharacterMovementForceMultiplier = 1;

	UPROPERTY()
	double EnterExitWaterTolerance = -35.0;

	UPROPERTY(Replicated)
	TObjectPtr<AOceanologyWaterParent> OceanologyWater = nullptr;

	UPROPERTY(Replicated)
	TObjectPtr<AOceanologyWaterVolume> OceanologyWaterVolume = nullptr;

	UFUNCTION(BlueprintPure, Category="Buoyancy")
	AOceanologyWaterParent* GetCurrentWater() const { return OceanologyWater; }

	UFUNCTION(BlueprintPure, Category="Buoyancy")
	AOceanologyWaterVolume* GetCurrentWaterVolume() const { return OceanologyWaterVolume; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Buoyancy")
	UCharacterMovementComponent* GetCharacterMovement();

	UFUNCTION(BlueprintCallable, Category="Buoyancy|Flow")
	void SetFlowControlSpline(
		USplineComponent* InFlowControlSpline,
		const double InUnscaledSplineWidth
	);

	UFUNCTION(BlueprintPure, Category="Buoyancy|Flow")
	USplineComponent* GetFlowControlSpline() const;

protected:
	virtual void TickComponent(
		const float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> OwnerMesh = nullptr;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovement = nullptr;

	UPROPERTY()
	TObjectPtr<USplineComponent> FlowControlSpline = nullptr;

	UPROPERTY(Replicated)
	FVector CurrentWaveHeight = FVector::ZeroVector;

	bool FirstInit = true;
	double RadiusWithGravitySigned = 0;
	double OwnerMeshAngularDamping = 0;
	double OwnerMeshLinearDamping = 0;
	double GlobalWaveDirectionX = 0;
	double GlobalWaveDirectionY = 0;
	double UnscaledSplineWidth = 75.0;

	void ApplyBuoyancyToMesh();
	void ApplyFlowToMesh();
	float GetMass() const;
	float GetGravityZ() const;
};
