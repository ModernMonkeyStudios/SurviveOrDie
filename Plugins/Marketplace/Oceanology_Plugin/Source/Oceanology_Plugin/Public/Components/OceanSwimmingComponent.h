// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "CharacterMovementComponentAsync.h"
#include "Components/ActorComponent.h"
#include "OceanSwimmingComponent.generated.h"

class AOceanologyWaterVolume;
class UWorld;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;
class AOceanologyWaterParent;
class UCharacterMovementComponent;
class UOceanBuoyancyComponent;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnStartSwimming);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnStopSwimming);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOceanOnUnderwaterTask, int32, TimeUnderwaterInSeconds);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnUnderwaterEnter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnUnderwaterExit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnStartDrowning);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnStopDrowning);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOceanOnDrowningTask, int32, TimeDrowningInSeconds);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOceanOnDrownDeath);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanSwimmingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanSwimmingComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming",
		meta = (ToolTip =
			"Value 0 means real-time swimming state checking. Can be changed, but might provide inaccuracy. This controls tick rate."
		))
	float SwimmingUpdateInterval = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming",
		meta = (ToolTip =
			"The name of the buoyancy pontoon that is used for enter water tracing events. A Buoyancy Pontoon must exist with this name with 'WaterEnterExitEventOnly' mode."
		))
	FName BuoyancyPontoonSocketForEnterWaterEvent = "Head";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming",
		meta = (ToolTip = "The buoyant force that pushes the character towards the water surface."))
	double SwimmingBuoyancyForceMultiplier = 0.03;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming",
		meta = (ToolTip = "The immersion depth of the character in which it starts/stops swimming."))
	double Immersion = 100.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming",
		meta = (ToolTip = "This configuration limits how high you can swim up."))
	double SwimUpLimiter = 10.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Speed",
		meta = (ToolTip = "How fast the character swims during regular swimming."))
	float SwimMaxSpeed = 300.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Speed",
		meta = (ToolTip = "How fast the character swims during swim sprinting."))
	float SwimFastMaxSpeed = 600.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming",
		meta = (ToolTip = "This configuration limits how high the surface locked swimming happens."))
	double SurfaceLockedSwimmingLimiter = 50.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming",
		meta = (ToolTip =
			"By default the enter/exit water tolerance works with the collision capsule's scaled half height. With this you can add offset."
		))
	double EnterExitWaterToleranceOffset = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Drowning",
		meta = (ToolTip = "If enabled character will start drown, then eventually die of drowning."))
	bool DrowningEnabled = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Drowning",
		meta = (ToolTip = "After how many seconds the character will start drowning."))
	int32 DrowningTimeWarningInSeconds = 7;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Drowning",
		meta = (ToolTip = "After how many seconds the character will die of drowning."))
	int32 DrowningTimeDeathInSeconds = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects",
		meta = (ToolTip = "Special underwater effects control during swimming."))
	bool EnableUnderwaterEffects = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	TObjectPtr<UNiagaraSystem> HeadBubblesEffectNiagara = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects",
		meta = (ToolTip = "The head socket name in character's skeleton tree where the effect will spawn."))
	FName HeadSocketName = "head_Socket";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	TObjectPtr<UNiagaraSystem> RightFootBubblesEffectNiagara = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	TObjectPtr<UNiagaraSystem> LeftFootBubblesEffectNiagara = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects",
		meta = (ToolTip = "The right foot socket name in character's skeleton tree where the effect will spawn."))
	FName RightFootSocketName = "foot_r_Socket";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects",
		meta = (ToolTip = "The left foot socket name in character's skeleton tree where the effect will spawn."))
	FName LeftFootSocketName = "foot_l_Socket";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	bool StopHeadBubblesWhileSwimFast = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	bool StopRightFootBubblesWhileSwimFast = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	bool StopLeftFootBubblesWhileSwimFast = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects")
	TObjectPtr<UNiagaraSystem> SwimFastTrailEffectNiagara = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Effects",
		meta = (ToolTip = "The swim fast trail socket name in character's skeleton tree where the effect will spawn."))
	FName SwimFastTrailSocketName = "swim_fast_trail_Socket";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Underwater|Sound")
	TObjectPtr<USoundBase> SwimFastTrailSound = nullptr;

	/** Turning on/off debug points and messages support. WARNING: Debugging affects performance, do NOT use in production! */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Debug")
	bool DebugEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	bool EnteredWaterVolume = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	bool EnteredWater = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	bool Submerged = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	bool Swimming = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	int32 UnderwaterTimeCounter = 0;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	bool UnderwaterTaskRunning = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	int32 DrowningTimeCounter = 0;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|State", Replicated)
	bool DrowningTaskRunning = false;

	UPROPERTY(BlueprintReadWrite, Category="Swimming|State", Replicated)
	bool DrownedToDeath = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	bool SurfaceLockedSwimming = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	bool SwimDown = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	bool SwimUp = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	bool SwimFast = false;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	double SwimUpOrDownAxis = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	double LookUpAxis = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	double MoveForwardBackwardAxis = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="Swimming|Controls", Replicated)
	double MoveLeftRightAxis = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Controls",
		meta = (ToolTip =
			"Use orient rotation to movement during swimming? It will reset to original once you stop swimming."))
	bool SwimmingOrientRotationToMovement = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Swimming|Controls",
		meta = (ToolTip =
			"Use controller rotation yaw during swimming? It will reset to original once you stop swimming. Multi-dimension animations might require this type of config to be turned ON."
		))
	bool SwimmingUseControllerRotationYaw = false;

	UPROPERTY(BlueprintAssignable, Category="Swimming")
	FOceanOnStartSwimming OnStartSwimming;

	UPROPERTY(BlueprintAssignable, Category="Swimming")
	FOceanOnStartSwimming OnStopSwimming;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater")
	FOceanOnUnderwaterTask OnUnderwaterTask;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater")
	FOceanOnUnderwaterEnter OnUnderwaterEnter;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater")
	FOceanOnUnderwaterExit OnUnderwaterExit;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater|Drowning")
	FOceanOnStartDrowning OnStartDrowning;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater|Drowning")
	FOceanOnStopDrowning OnStopDrowning;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater|Drowning")
	FOceanOnDrowningTask OnDrowningTask;

	UPROPERTY(BlueprintAssignable, Category="Swimming|Underwater|Drowning")
	FOceanOnDrownDeath OnDrownDeath;

	UPROPERTY(Replicated)
	TObjectPtr<AOceanologyWaterParent> OceanologyWater = nullptr;

	UPROPERTY(Replicated)
	TObjectPtr<AOceanologyWaterVolume> OceanologyWaterVolume = nullptr;

	UFUNCTION(BlueprintPure, Category="Swimming")
	AOceanologyWaterParent* GetCurrentWater() const { return OceanologyWater; }

	UFUNCTION(BlueprintPure, Category="Buoyancy")
	AOceanologyWaterVolume* GetCurrentWaterVolume() const { return OceanologyWaterVolume; }

	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Swimming")
	void ToggleDebug() { DebugEnabled = !DebugEnabled; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category="Swimming|Controls")
	void Server_SurfaceLockedSwimming(const bool Value);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Swimming|Controls")
	void NetMulticast_SurfaceLockedSwimming(const bool Value);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category="Swimming|Controls")
	void Server_SwimFast(const bool Value);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Swimming|Controls")
	void NetMulticast_SwimFast(const bool Value);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category="Swimming|Controls")
	void Server_SwimUpOrDown(const bool Value, const double AxisValue);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Swimming|Controls")
	void NetMulticast_SwimUpOrDown(const bool Value, const double AxisValue);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category="Swimming|Controls")
	void Server_LookUp(const double AxisValue);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Swimming|Controls")
	void NetMulticast_LookUp(const double AxisValue);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category="Swimming|Controls")
	void Server_MoveForwardBackward(const double AxisValue);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Swimming|Controls")
	void NetMulticast_MoveForwardBackward(const double AxisValue);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category="Swimming|Controls")
	void Server_MoveLeftRight(const double AxisValue);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Swimming|Controls")
	void NetMulticast_MoveLeftRight(const double AxisValue);

	UFUNCTION(BlueprintPure, Category="Swimming")
	bool IsSurfaceLockedSwimming() const { return EffectiveSurfaceLockedSwimming; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Swimming")
	UCharacterMovementComponent* GetCharacterMovement();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Swimming")
	bool CanEnterWater(const FName Socket, const bool Underwater);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Swimming")
	bool CanExitWater(const FName Socket, const bool Underwater);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Swimming")
	bool CanStartSwim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Swimming")
	bool CanStopSwim();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Swimming")
	bool CanAlterState();

protected:
	UFUNCTION()
	void OnEnteredWater(const FName Socket, const bool Underwater);

	UFUNCTION()
	void OnExitedWater(const FName Socket, const bool Underwater);

	UFUNCTION()
	void ComputeWaterFloorHeight();

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;
	virtual void TickComponent(
		const float DeltaTime,
		const ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	UPROPERTY(Replicated)
	FFindFloorResult FindFloorResult;

	UPROPERTY(Replicated)
	float OriginalMaxWalkSpeed = 0;

	UPROPERTY(Replicated)
	float OriginalMaxSwimSpeed = 0;

	UPROPERTY(Replicated)
	bool OriginalOrientRotationToMovement = false;

	UPROPERTY(Replicated)
	bool OriginalUseControllerRotationYaw = false;

	UPROPERTY(Replicated)
	double OriginalBuoyancy = 1;

	UPROPERTY(Replicated)
	double EffectiveSwimmingBuoyancyForce = SwimmingBuoyancyForceMultiplier;

	UPROPERTY(Replicated)
	bool EffectiveSurfaceLockedSwimming = false;

	UPROPERTY(Replicated)
	TEnumAsByte<enum EMovementMode> LastMovementMode = MOVE_Walking;

	UPROPERTY()
	FTimerHandle UnderwaterTaskDelegate;

	UPROPERTY()
	FTimerHandle DrowningTaskDelegate;

	UPROPERTY()
	UNiagaraComponent* HeadBubblesEffect = nullptr;

	UPROPERTY()
	UNiagaraComponent* RightFootBubblesEffect = nullptr;

	UPROPERTY()
	UNiagaraComponent* LeftFootBubblesEffect = nullptr;

	UPROPERTY()
	UNiagaraComponent* SwimFastTrailEffect = nullptr;

	UPROPERTY(Category="Material Instance Dynamic", VisibleInstanceOnly, Transient)
	UMaterialInstanceDynamic* SwimmingBubblesMID = nullptr;

	void HandleSwimming();
	void StartSwimming();
	void StopSwimming();
	bool ShouldSwim() const;
	void HandleStateAlteration();

	void SetupUnderwaterEffects();
	void HandleWaterSurface(const float DeltaTime);
	void HandleUnderwaterEffects();

	void StartHeadBubblesEffect(USkeletalMeshComponent* MeshComponent);
	void StartRightFootBubblesEffect(USkeletalMeshComponent* MeshComponent);
	void StartLeftFootBubblesEffect(USkeletalMeshComponent* MeshComponent);
	void StartSwimFastTrailEffect(USkeletalMeshComponent* MeshComponent);

	void StopHeadBubblesEffect();
	void StopRightFootBubblesEffect();
	void StopLeftFootBubblesEffect();
	void StopSwimFastTrailEffect();

	void HandleUnderwaterTimers();

	void StartUnderwaterTask();
	void StopUnderwaterTask();
	void StartDrowningTask();
	void StopDrowningTask();

	UFUNCTION()
	void UnderwaterTask();

	UFUNCTION()
	void DrowningTask();

	UFUNCTION()
	void DrownToDeath();
};
