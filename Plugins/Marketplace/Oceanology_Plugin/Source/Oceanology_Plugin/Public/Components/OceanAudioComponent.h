// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "OceanAudioComponent.generated.h"

class AOceanologyWaterParent;

UENUM(BlueprintType)
enum class EOceanAudioMode : uint8
{
	None,
	Underwater,
	Wave
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UOceanAudioComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ocean Audio",
		meta = (ToolTip =
			"Control the mode of the Ocean Audio component. None = Disabled, Underwater = sound below waves, Waves = sound above waves"
		))
	EOceanAudioMode Mode = EOceanAudioMode::None;

	/** Turning on/off debug messages of the water audio. WARNING: Debugging affects performance, do NOT use in production! */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ocean Audio|Debug")
	bool DebugEnabled = false;

	void Init();
	void InitSound();

	UFUNCTION(Category="Sound FX")
	void PlaySoundFX();

	virtual void Activate(const bool bReset) override;
	virtual void Deactivate() override;

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		const float DeltaTime,
		const ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	UPROPERTY()
	TObjectPtr<AOceanologyWaterParent> OceanologyWater;
	
	bool ActiveAudio = true;
};
