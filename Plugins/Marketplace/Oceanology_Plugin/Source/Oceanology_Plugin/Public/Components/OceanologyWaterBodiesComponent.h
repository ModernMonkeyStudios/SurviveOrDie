// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanologyWaterBodiesComponent.generated.h"

class UWorld;
class UMaterialParameterCollectionInstance;
class UTextureRenderTarget2D;
class UBoxComponent;
class UVirtualTexture2D;
class UVirtualTextureBuilder;
class UTexture2D;

UCLASS()
class OCEANOLOGY_PLUGIN_API UOceanologyWaterBodiesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanologyWaterBodiesComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WaterBodies,
		meta = (AllowPrivateAccess = "true", AdvancedDisplay))
	bool bUseCustomRVTBaker = true;

	/** Number of streaming low mips to build for the virtual texture. */
	UPROPERTY(EditAnywhere, Category = WaterBodies, meta = (UIMin = "0", UIMax = "12", DisplayName = "Build Levels"))
	int32 BuildLevels = 3;

#if WITH_EDITORONLY_DATA
	// Approximate size of built water bodies texture
	// After changing RVT settings, this will update after changing BuildLevels
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WaterBodies, Transient)
	int32 TextureSize = 0;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WaterBodies)
	TObjectPtr<UTextureRenderTarget2D> WaterBodiesRenderTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Transient, Category = WaterBodies, AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> CopyMaterialInstance;

	UFUNCTION()
	void Init();

	UFUNCTION(BlueprintCallable, Category=WaterBodies)
	void CaptureRT();

private:
	UPROPERTY(VisibleAnywhere, Transient, Category = WaterBodies, AdvancedDisplay)
	TObjectPtr<UTexture2D> BakedWaterBodies;
};
