// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Structs/OceanologyWaterPresetResult.h"
#include "Components/ActorComponent.h"
#include "UObject/Object.h"
#include "OceanologyWaveSolverComponent.generated.h"

UENUM(BlueprintType)
enum class EWaveTimeMode : uint8
{
	LocalTime,
	ServerTimeForReplication
};

class AOceanologyWaterParent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UMaterialParameterCollection;

UCLASS(BlueprintType, Blueprintable, Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),
	hidecategories = (Object, Activation, "Components|Activation", Collision, Lighting, HLOD, Navigation, Replication,
		Input, MaterialParameters, TextureStreaming, Tags, AssetUserData, Cooking))
class OCEANOLOGY_PLUGIN_API UOceanologyWaveSolverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanologyWaveSolverComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	EWaveTimeMode WaveTimeMode = EWaveTimeMode::ServerTimeForReplication;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float WaveSolverUpdateInterval = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Waves")
	void InitWaves(
		AOceanologyWaterParent* InOwner,
		const bool InUsingMPC_Waves
	);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Waves")
	void UpdateWaves();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Waves")
	void UpdateWavesByPresetResult(
		AOceanologyWaterParent* Water,
		const FOceanologyWaterPresetResult& WaterPresetResult
	);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Waves")
	FVector GetWaveHeightAtLocation(const FVector& Location);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Waves")
	double GetMinWaveHeight();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Waves")
	double GetMaxWaveHeight();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Waves")
	double GetGlobalWaveDirection();

	UFUNCTION(BlueprintPure, Category="Waves")
	bool IsUsingMPC_Waves() const;

	UFUNCTION(Category = "Waves", BlueprintCallable)
	void StartWaveSimulation();

	UFUNCTION(Category = "Waves", BlueprintCallable)
	void StopWaveSimulation();

protected:
	virtual void TickComponent(
		const float DeltaTime,
		const ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void SetGameTimeInSeconds();

protected:
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Waves")
	AOceanologyWaterParent* Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Waves")
	UMaterialParameterCollection* MPC_Waves = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Waves")
	double GameTimeInSeconds = 0;

	UPROPERTY(BlueprintReadOnly, Category="Waves")
	bool SimulateWaves = true;

private:
	UPROPERTY(Transient)
	AOceanologyWaterParent* FirstWaterInstance = nullptr;

	bool UsingMPC_Waves = false;
};
