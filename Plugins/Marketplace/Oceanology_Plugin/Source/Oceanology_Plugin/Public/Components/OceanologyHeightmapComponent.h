// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanologyHeightmapComponent.generated.h"


class UWorld;
class UMaterialParameterCollectionInstance;
class UTextureRenderTarget2D;
class UBoxComponent;
class UVirtualTexture2D;
class UVirtualTextureBuilder;
class UTexture2D;

UCLASS(HideCategories =(AssetUserData, Activation, Tags, Cooking, Navigation), ClassGroup=(Custom),
	meta=(BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanologyHeightmapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanologyHeightmapComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Heightmap, meta = (AllowPrivateAccess = "true"))
	bool bUseRenderTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Heightmap,
		meta = (AllowPrivateAccess = "true", EditCondition = "!bUseRenderTarget", EditConditionHides))
	TObjectPtr<UTexture2D> HeightmapTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Heightmap,
		meta = (AllowPrivateAccess = "true", EditCondition = "bUseRenderTarget", EditConditionHides))
	TObjectPtr<UTextureRenderTarget2D> HeightmapRenderTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Heightmap,
		meta = (AllowPrivateAccess = "true", AdvancedDisplay))
	bool bUseCustomRVTBaker = true;

	/** Number of streaming low mips to build for the virtual texture. */
	UPROPERTY(EditAnywhere, Category = Heightmap, meta = (UIMin = "0", UIMax = "12", DisplayName = "Build Levels"))
	int32 BuildLevels = 3;

#if WITH_EDITORONLY_DATA
	// Approximate size of built heightmap texture
	// After changing RVT settings, this will update after changing BuildLevels
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Heightmap, Transient)
	int32 TextureSize = 0;
#endif

	UPROPERTY(VisibleAnywhere, Transient, Category = Heightmap, AdvancedDisplay)
	TObjectPtr<UTexture2D> BakedHeightmap;

	UPROPERTY(VisibleAnywhere, Transient, Category = Heightmap, AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> JumpFloodMaterialInstance;

	UPROPERTY(VisibleAnywhere, Transient, Category = Heightmap, AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> CopyUVsMaterialInstance;

	UPROPERTY(VisibleAnywhere, Transient, Category = Heightmap, AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> ComputeSDFMaterialInstance;

	UPROPERTY(VisibleAnywhere, Transient, Category = Heightmap, AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> BlurMaterialInstance;

	UPROPERTY(VisibleAnywhere, Transient, Category = Heightmap, AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> ComposeMaterialInstance;

	UFUNCTION()
	void SaveHeightmap(UTexture2D* NewHeightmap);

	UFUNCTION(Category = "Oceanology", BlueprintCallable)
	void SaveHeightmapRenderTarget();

	UFUNCTION()
	void SetupLandscapeOrGroundMesh();

	UFUNCTION()
	void SetupWater();

	UFUNCTION()
	void SetupWaterMinMax();

	UFUNCTION()
	void Init();

private:
	void BakeHeightmapIntoRenderTarget(UTextureRenderTarget2D* Target);
	void BakeSDFIntoRenderTarget(UTexture* Heightmap, UTextureRenderTarget2D* Target, const float OceanHeightZ);
};
