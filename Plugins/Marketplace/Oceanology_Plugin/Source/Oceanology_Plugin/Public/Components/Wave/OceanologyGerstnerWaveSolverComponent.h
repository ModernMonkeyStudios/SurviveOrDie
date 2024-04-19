// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "OceanologyWaveSolverComponent.h"
#include "Actors/OceanologyWaterParentActor.h"
#include "Enums/OceanologyGerstnerWaveSummarize.h"
#include "Structs/OceanologyBaseOffset.h"
#include "Structs/OceanologyGlobalDisplacement.h"
#include "Structs/OceanologyLakePresetResult.h"
#include "Structs/OceanologyOceanPresetResult.h"
#include "Structs/OceanologyWaterPresetResult.h"
#include "Structs/OceanologyWaves.h"
#include "OceanologyGerstnerWaveSolverComponent.generated.h"

class UWorld;
class UMaterialParameterCollection;
class UMaterialInstanceDynamic;

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanologyGerstnerWaveSolverComponent : public UOceanologyWaveSolverComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Global )", Replicated)
	FOceanologyGlobalDisplacement GlobalDisplacement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", Replicated)
	FOceanologyBaseOffset BaseOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ1",
		Replicated)
	FOceanologyWave_1 Wave_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ2",
		Replicated)
	FOceanologyWave_2 Wave_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ3",
		Replicated)
	FOceanologyWave_3 Wave_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves|Gerstner Waves ( Custom )", DisplayName="Σ4",
		Replicated)
	FOceanologyWave_4 Wave_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gerstner Waves", Replicated)
	EOceanologyGerstnerWaveSummarize Summarize = EOceanologyGerstnerWaveSummarize::Waves_8;

	virtual void InitWaves_Implementation(
		AOceanologyWaterParent* InOwner,
		const bool InUsingMPC_Waves
	) override;
	virtual void UpdateWaves_Implementation() override;
	void UpdateWavesByMPC() const;
	void UpdateWavesByMaterialParameters() const;

	virtual void UpdateWavesByPresetResult_Implementation(
		AOceanologyWaterParent* Water,
		const FOceanologyWaterPresetResult& WaterPresetResult
	) override;

	UFUNCTION(BlueprintNativeEvent, Category="Waves")
	void UpdateLakeWavesByPresetResult(
		const FOceanologyWaterPresetResult& WaterPresetResult,
		const FOceanologyLakePresetResult& LakePresetResult
	);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Waves")
	void UpdateOceanWavesByPresetResult(
		const FOceanologyWaterPresetResult& WaterPresetResult,
		const FOceanologyOceanPresetResult& OceanPresetResult
	);

	virtual FVector GetWaveHeightAtLocation_Implementation(const FVector& Location) override;
	virtual double GetMaxWaveHeight_Implementation() override;
	virtual double GetGlobalWaveDirection_Implementation() override;

	UFUNCTION(BlueprintPure, Category="Gerstner Waves")
	FVector GerstnerWavesQuadruple(
		const FVector& Position,
		const double Amplitude,
		const double Steepness,
		const double Speed,
		const double WaveLength,
		const double Direction,
		const double NumWaves,
		const double DirectionOffset
	);

	UFUNCTION(BlueprintPure, Category="Gerstner Waves")
	FVector GerstnerWave(
		const FVector& Position,
		const double Direction,
		const double Speed,
		const double WaveLength,
		const double Amplitude,
		const double Steepness,
		const double NumWaves
	);

	UFUNCTION(BlueprintPure, Category="Gerstner Waves")
	static double DivideWave(const double Value);

	UFUNCTION(BlueprintPure, Category="Gerstner Waves")
	static double MultiplyWave(const double Value);

	UFUNCTION(BlueprintPure, Category="Gerstner Waves")
	static FVector DirectionWave(const double Direction);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
