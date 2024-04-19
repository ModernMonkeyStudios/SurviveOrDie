// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PhysicsVolume.h"
#include "OceanologyWaterVolumeActor.generated.h"

class AOceanologyWaterParent;
class UBoxComponent;

UCLASS(HideCategories=(CharacterMovement, Volume, HLOD))
class OCEANOLOGY_PLUGIN_API AOceanologyWaterVolume : public APhysicsVolume
{
	GENERATED_BODY()

public:
	AOceanologyWaterVolume();

	/** Turning on/off debug messages of the water volume. WARNING: Debugging affects performance, do NOT use in production! */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Debug")
	bool DebugEnabled = false;

	/** The owner water of this water volume. Mandatory. Used to determine wave height. Physics & swimming logic's fundamental settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", Replicated)
	TObjectPtr<AOceanologyWaterParent> OceanologyWater = nullptr;

	/** Actor to align rotation to. If set this actor is always included in the bounds calculation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	TSoftObjectPtr<AActor> BoundsAlignActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	bool CheckInitialOverlapOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	double InitialOverlapOnBeginPlayDelay = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	bool EnableBuoyancyInArea = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	bool EnableSwimmingInArea = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (EditCondition = "EnableSwimmingInArea"))
	float PhysicsVolumeTerminalVelocity = 4000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (EditCondition = "EnableSwimmingInArea"))
	int32 PhysicsVolumePriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (EditCondition = "EnableSwimmingInArea"))
	float PhysicsVolumeFluidFriction = 0.3;

#if WITH_EDITOR
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif

	void Init();

	UFUNCTION()
	void RegisterOverlapEvents();

	UFUNCTION()
	void InitialOverlappingCheck();

	UFUNCTION()
	void OnVolume_BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnVolume_EndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION(Server, Reliable, Category="Water Volume")
	void Server_ChangeOverlapStatus(const AActor* OtherActor, const bool Active);

	UFUNCTION(NetMulticast, Reliable, Category="Water Volume")
	void NetMulticast_ChangeOverlapStatus(const AActor* OtherActor, const bool Active);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
