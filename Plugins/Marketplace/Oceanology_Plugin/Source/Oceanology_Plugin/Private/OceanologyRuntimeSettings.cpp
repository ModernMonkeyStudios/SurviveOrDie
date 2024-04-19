// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "OceanologyRuntimeSettings.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraSystem.h"
#include "Components/Wave/OceanologyGerstnerWaveSolverComponent.h"
#include "Components/Wave/OceanologyEmptyWaveSolverComponent.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/StaticMesh.h"
#include "VT/RuntimeVirtualTexture.h"
#include "VT/VirtualTextureBuilder.h"
#include "Sound/SoundBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OceanologyRuntimeSettings)


#if WITH_EDITOR
UOceanologyRuntimeSettings::FOnUpdateSettings UOceanologyRuntimeSettings::OnSettingsChange;
#endif //WITH_EDITOR

UOceanologyRuntimeSettings::UOceanologyRuntimeSettings():
	LakeWaveSolverClass(UOceanologyGerstnerWaveSolverComponent::StaticClass()),
	OceanWaveSolverClass(UOceanologyGerstnerWaveSolverComponent::StaticClass()),

	LakeMesh(
		FSoftObjectPath(TEXT(
			"/Engine/BasicShapes/Plane.Plane"))
	),

	VolumetricFogMesh(
		FSoftObjectPath(TEXT(
			"/Engine/BasicShapes/Cube.Cube"))
	),

	LakeBubblesMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Effects/Particles/Water/Materials/M_LakeBubbles_Inst.M_LakeBubbles_Inst"))
	),

	LakeGerstnerWaveMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Lake/Materials/Water/SingleLayerWater/M_Lake_Inst.M_Lake_Inst"))
	),
	LakeGerstnerWaveFarMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Lake/Materials/Water/SingleLayerWater/M_Lake_Inst.M_Lake_Inst"))
	),

	LakeUnderwaterMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Lake/Materials/PostProcess/Underwater/M_UnderLake_PostProcess_Volume_Inst.M_UnderLake_PostProcess_Volume_Inst"))
	),
	LakeUnderwaterColorlessMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Lake/Materials/PostProcess/Underwater/M_UnderLake_PostProcess_Colorless_Inst.M_UnderLake_PostProcess_Colorless_Inst"))
	),
	LakeVolumetricFogMaterial(
		FSoftObjectPath(
			TEXT(
				"/Oceanology_Plugin/Design/Lake/Materials/VolumetricFog/M_UnderLake_VolumetricFog_Inst.M_UnderLake_VolumetricFog_Inst"))
	),
	LakeCausticsMaterial(
		FSoftObjectPath(
			TEXT(
				"/Oceanology_Plugin/Design/Ocean/Materials/Decal/CausticsDL/M_Caustics_Decal_Inst.M_Caustics_Decal_Inst"))
	),

	OceanBubblesMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Effects/Particles/Water/Materials/M_OceanBubbles_Inst.M_OceanBubbles_Inst"))
	),

	OceanGerstnerWaveMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Ocean/Materials/Water/SingleLayerWater/M_Oceanology_Inst.M_Oceanology_Inst"))
	),
	OceanGerstnerWaveFarMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Ocean/Materials/Water/SingleLayerWater/M_Oceanology_Far_Inst.M_Oceanology_Far_Inst"))
	),
	OceanUnderwaterMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Ocean/Materials/PostProcess/Underwater/M_UnderOcean_PostProcess_Volume_Inst.M_UnderOcean_PostProcess_Volume_Inst"))
	),
	OceanUnderwaterColorlessMaterial(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Design/Ocean/Materials/PostProcess/Underwater/M_UnderOcean_PostProcess_Colorless_Inst.M_UnderOcean_PostProcess_Colorless_Inst"))
	),
	OceanVolumetricFogMaterial(
		FSoftObjectPath(
			TEXT(
				"/Oceanology_Plugin/Design/Ocean/Materials/VolumetricFog/M_UnderOcean_VolumetricFog_Inst.M_UnderOcean_VolumetricFog_Inst"))
	),
	OceanShorelineWetnessMaterial(
		FSoftObjectPath(
			TEXT(
				"/Oceanology_Plugin/Design/Ocean/Materials/Decal/ShorelineWetness/M_ShorelineWetness_Inst.M_ShorelineWetness_Inst"))
	),

	MPC_Waves(
		FSoftObjectPath(TEXT("/Oceanology_Plugin/Design/Ocean/MaterialCollections/MPC_Waves.MPC_Waves"))
	),
	Cue_Wave_Ocean(
		FSoftObjectPath(TEXT("/Oceanology_Plugin/Design/Audio/Cue/Cue_Wave_Ocean.Cue_Wave_Ocean"))
	),
	Cue_Underwater_Ocean(
		FSoftObjectPath(TEXT("/Oceanology_Plugin/Design/Audio/Cue/Cue_Underwater_Ocean.Cue_Underwater_Ocean"))
	),
	Cue_Wave_Lake(
		FSoftObjectPath(TEXT("/Oceanology_Plugin/Design/Audio/Cue/Cue_Wave_Lake.Cue_Wave_Lake"))
	),
	Cue_Underwater_Lake(
		FSoftObjectPath(TEXT("/Oceanology_Plugin/Design/Audio/Cue/Cue_Underwater_Lake.Cue_Underwater_Lake"))
	),
	RVT_Heightmap(
		FSoftObjectPath(TEXT("/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/RVT_Heightmap.RVT_Heightmap"))),
	RVT_Heightmap_Builder(FSoftObjectPath(
		TEXT("/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/RVT_Heightmap_SVT.RVT_Heightmap_SVT"))),
	RT_SDF_CopyUVs(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/M_RT_SDF_CopyUVs.M_RT_SDF_CopyUVs")
	)),
	RT_SDF_JumpFlood(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/M_RT_SDF_JumpFlood.M_RT_SDF_JumpFlood")
	)),
	RT_SDF_ComputeSDF(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/M_RT_SDF_ComputeSDF.M_RT_SDF_ComputeSDF")
	)),
	RT_SDF_Blur(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/M_RT_SDF_Blur.M_RT_SDF_Blur")
	)),
	RT_SDF_Compose(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/M_RT_SDF_Compose.M_RT_SDF_Compose")
	)),

	RVT_WaterBodies(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/RVT_WaterBodies.RVT_WaterBodies"))),
	RVT_WaterBodies_Builder(FSoftObjectPath(
		TEXT("/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/RVT_WaterBodies_SVT.RVT_WaterBodies_SVT"))),

	RVT_WaterBodies_CopyMaterial(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/MI_WaterBodies_CopyMaterial.MI_WaterBodies_CopyMaterial"))),

	RT_WaterBodies(FSoftObjectPath(
		TEXT(
			"/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/RVT/RT_WaterBodies.RT_WaterBodies"))),

	LakeBubblesEffect(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Effects/Particles/Water/NS_LakeBurbblesUnderwater.NS_LakeBurbblesUnderwater"))
	),
	OceanBubblesEffect(
		FSoftObjectPath(TEXT(
			"/Oceanology_Plugin/Effects/Particles/Water/NS_OceanBurbblesUnderwater.NS_OceanBurbblesUnderwater"))
	),

	HeadBubblesEffectNiagara(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Effects/Particles/Water/NS_Head_Bubbles.NS_Head_Bubbles"))
	),

	RightFootBubblesEffectNiagara(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Effects/Particles/Water/NS_Foot_Bubbles.NS_Foot_Bubbles"))
	),

	LeftFootBubblesEffectNiagara(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Effects/Particles/Water/NS_Foot_Bubbles.NS_Foot_Bubbles"))
	),

	SwimFastTrailEffectNiagara(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Effects/Particles/Water/NS_SwimFast_Trail.NS_SwimFast_Trail"))
	),

	SwimFastTrailSound(nullptr),

	NiagaraGerstnerWavesEffect(
		FSoftObjectPath(
			TEXT("/Oceanology_Plugin/Design/Ocean/Blueprints/Effects/Niagara/NS_GerstnerWaves.NS_GerstnerWaves"))
	)
{
}

#if WITH_EDITOR
void UOceanologyRuntimeSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnSettingsChange.Broadcast(this, PropertyChangedEvent.ChangeType);
}
#endif // WITH_EDITOR

void UOceanologyRuntimeSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

FName UOceanologyRuntimeSettings::GetCategoryName() const
{
	return FName(TEXT("Oceanology Plugin"));
}

TSubclassOf<UOceanologyWaveSolverComponent> UOceanologyRuntimeSettings::GetLakeWaveSolverClass() const
{
	return LakeWaveSolverClass;
}

TSubclassOf<UOceanologyWaveSolverComponent> UOceanologyRuntimeSettings::GetOceanWaveSolverClass() const
{
	return OceanWaveSolverClass;
}

UStaticMesh* UOceanologyRuntimeSettings::GetLakeMesh() const
{
	return LakeMesh.LoadSynchronous();
}

UStaticMesh* UOceanologyRuntimeSettings::GetVolumetricFogMesh() const
{
	return VolumetricFogMesh.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeBubblesMaterial() const
{
	return LakeBubblesMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeGerstnerWaveMaterial() const
{
	return LakeGerstnerWaveMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeGerstnerWaveFarMaterial() const
{
	return LakeGerstnerWaveFarMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeUnderwaterMaterial() const
{
	return LakeUnderwaterMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeUnderwaterColorlessMaterial() const
{
	return LakeUnderwaterColorlessMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeVolumetricFogMaterial() const
{
	return LakeVolumetricFogMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetLakeCausticsMaterial() const
{
	return LakeCausticsMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanBubblesMaterial() const
{
	return OceanBubblesMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanGerstnerWaveMaterial() const
{
	return OceanGerstnerWaveMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanGerstnerWaveFarMaterial() const
{
	return OceanGerstnerWaveFarMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanUnderwaterMaterial() const
{
	return OceanUnderwaterMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanUnderwaterColorlessMaterial() const
{
	return OceanUnderwaterColorlessMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanVolumetricFogMaterial() const
{
	return OceanVolumetricFogMaterial.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetOceanShorelineWetnessMaterial() const
{
	return OceanShorelineWetnessMaterial.LoadSynchronous();
}

UMaterialParameterCollection* UOceanologyRuntimeSettings::GetMPC_Waves() const
{
	return MPC_Waves.LoadSynchronous();
}

USoundBase* UOceanologyRuntimeSettings::GetCue_Wave_Ocean() const
{
	return Cue_Wave_Ocean.LoadSynchronous();
}

USoundBase* UOceanologyRuntimeSettings::GetCue_Underwater_Ocean() const
{
	return Cue_Underwater_Ocean.LoadSynchronous();
}

USoundBase* UOceanologyRuntimeSettings::GetCue_Wave_Lake() const
{
	return Cue_Wave_Lake.LoadSynchronous();
}

USoundBase* UOceanologyRuntimeSettings::GetCue_Underwater_Lake() const
{
	return Cue_Underwater_Lake.LoadSynchronous();
}

URuntimeVirtualTexture* UOceanologyRuntimeSettings::GetRVT_Heightmap() const
{
	return RVT_Heightmap.LoadSynchronous();
}

UVirtualTextureBuilder* UOceanologyRuntimeSettings::GetRVT_Heightmap_Builder() const
{
	return RVT_Heightmap_Builder.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetRT_SDF_CopyUVs() const
{
	return RT_SDF_CopyUVs.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetRT_SDF_JumpFlood() const
{
	return RT_SDF_JumpFlood.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetRT_SDF_ComputeSDF() const
{
	return RT_SDF_ComputeSDF.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetRT_SDF_Blur() const
{
	return RT_SDF_Blur.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetRT_SDF_Compose() const
{
	return RT_SDF_Compose.LoadSynchronous();
}

URuntimeVirtualTexture* UOceanologyRuntimeSettings::GetRVT_WaterBodies() const
{
	return RVT_WaterBodies.LoadSynchronous();
}

UVirtualTextureBuilder* UOceanologyRuntimeSettings::GetRVT_WaterBodies_Builder() const
{
	return RVT_WaterBodies_Builder.LoadSynchronous();
}

UTextureRenderTarget2D* UOceanologyRuntimeSettings::GetRT_WaterBodies() const
{
	return RT_WaterBodies.LoadSynchronous();
}

UMaterialInterface* UOceanologyRuntimeSettings::GetRVT_WaterBodies_CopyMaterial() const
{
	return RVT_WaterBodies_CopyMaterial.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetLakeBubblesEffect() const
{
	return LakeBubblesEffect.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetOceanBubblesEffect() const
{
	return OceanBubblesEffect.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetHeadBubblesEffectNiagara() const
{
	return HeadBubblesEffectNiagara.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetRightFootBubblesEffectNiagara() const
{
	return RightFootBubblesEffectNiagara.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetLeftFootBubblesEffectNiagara() const
{
	return LeftFootBubblesEffectNiagara.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetSwimFastTrailEffectNiagara() const
{
	return SwimFastTrailEffectNiagara.LoadSynchronous();
}

USoundBase* UOceanologyRuntimeSettings::GetSwimFastTrailSound() const
{
	return SwimFastTrailSound.LoadSynchronous();
}

UNiagaraSystem* UOceanologyRuntimeSettings::GetNiagaraGerstnerWavesEffect() const
{
	return NiagaraGerstnerWavesEffect.LoadSynchronous();
}
