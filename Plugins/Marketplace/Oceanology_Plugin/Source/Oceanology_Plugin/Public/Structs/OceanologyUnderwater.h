#pragma once

#include "Engine/Texture.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyUnderwater.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyUnderwater
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="AtmosphereLighting")
	double AtmosphereLighting = 25.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="Band Offset")
	double BandOffset = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", meta = (ClampMax = "2.0"), DisplayName="Band Opacity")
	double BandOpacity = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="Band Width")
	double BandWidth = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="DarknessFadeDepth")
	double DarknessFadeDepth = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="DeepColorDistance")
	double DeepColorDistance = 0.85;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", meta = (ClampMin = "1.0",ClampMax = "10.0"), DisplayName="DepthColorAbsorption")
	double DepthColorAbsorption = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="DistortionIntensity")
	double DistortionIntensity = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="DistortionScale")
	double DistortionScale = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="DistortionSpeed")
	double DistortionSpeed = 0.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", meta = (ClampMin = "0.01",ClampMax = "1.0"), DisplayName="FogDensity")
	double FogDensity = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="LocalPosition")
	double LocalPosition = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", meta = (ClampMin = "1.0",ClampMax = "20.0"), DisplayName="SunPower")
	double SunPower = 6.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", meta = (ClampMin = "100.0",ClampMax = "50000.0"), DisplayName="UnderwaterLightDepth")
	double UnderwaterLightDepth = 50000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="UnderwaterNoLightDepth")
	double UnderwaterNoLightDepth = 100000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="VignetteDark")
	double VignetteDark = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="VignetteDistortion")
	double VignetteDistortion = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="VignetteEdge")
	double VignetteEdge = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="WaterLineColorIntensity")
	double WaterLineColorIntensity = 4.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="WaterLineDistanceFalloff")
	double WaterLineDistanceFalloff = -0.015;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="WaterLineRefraction")
	double WaterLineRefraction = 0.005;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="WaterLineRefractionWidth")
	double WaterLineRefractionWidth = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="Wet Alpha")
	double WetAlpha = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="Wet Hardness")
	double WetHardness = 0.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="Wet Location")
	double WetLocation = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="Wet Radius")
	double WetRadius = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="ExponentialFogColorHigh")
	FLinearColor ExponentialFogColorHigh = FLinearColor(0.002797, 0.015643, 0.035, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="ExponentialFogColorLow")
	FLinearColor ExponentialFogColorLow = FLinearColor(0.0, 0.035, 0.02553, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="SceneColorDeep")
	FLinearColor SceneColorDeep = FLinearColor(0.138695, 1.0, 0.715861, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="SceneColorShallow")
	FLinearColor SceneColorShallow = FLinearColor(1.0, 1.0, 1.0, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Underwater", DisplayName="DistortionTexture")
	UTexture* DistortionTexture = nullptr;

	FOceanologyUnderwater()
	{
		DistortionTexture = LoadObject<UTexture>(nullptr, TEXT("/Engine/Functions/Engine_MaterialFunctions02/ExampleContent/Textures/WindTurblenceVectorAndGustMagnitude.WindTurblenceVectorAndGustMagnitude"), nullptr, LOAD_None, nullptr);
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyUnderwaterHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Underwater")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyUnderwater& Underwater)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("AtmosphereLighting", Underwater.AtmosphereLighting);
		MID->SetScalarParameterValue("Band Offset", Underwater.BandOffset);
		MID->SetScalarParameterValue("Band Opacity", Underwater.BandOpacity);
		MID->SetScalarParameterValue("Band Width", Underwater.BandWidth);
		MID->SetScalarParameterValue("DarknessFadeDepth", Underwater.DarknessFadeDepth);
		MID->SetScalarParameterValue("DeepColorDistance", Underwater.DeepColorDistance);
		MID->SetScalarParameterValue("DepthColorAbsorption", Underwater.DepthColorAbsorption);
		MID->SetScalarParameterValue("DistortionIntensity", Underwater.DistortionIntensity);
		MID->SetScalarParameterValue("DistortionScale", Underwater.DistortionScale);
		MID->SetScalarParameterValue("DistortionSpeed", Underwater.DistortionSpeed);
		MID->SetScalarParameterValue("FogDensity", Underwater.FogDensity);
		MID->SetScalarParameterValue("LocalPosition", Underwater.LocalPosition);
		MID->SetScalarParameterValue("SunPower", Underwater.SunPower);
		MID->SetScalarParameterValue("UnderwaterLightDepth", Underwater.UnderwaterLightDepth);
		MID->SetScalarParameterValue("UnderwaterNoLightDepth", Underwater.UnderwaterNoLightDepth);
		MID->SetScalarParameterValue("VignetteDark", Underwater.VignetteDark);
		MID->SetScalarParameterValue("VignetteDistortion", Underwater.VignetteDistortion);
		MID->SetScalarParameterValue("VignetteEdge", Underwater.VignetteEdge);
		MID->SetScalarParameterValue("WaterLineColorIntensity", Underwater.WaterLineColorIntensity);
		MID->SetScalarParameterValue("WaterLineDistanceFalloff", Underwater.WaterLineDistanceFalloff);
		MID->SetScalarParameterValue("WaterLineRefraction", Underwater.WaterLineRefraction);
		MID->SetScalarParameterValue("WaterLineRefractionWidth", Underwater.WaterLineRefractionWidth);
		MID->SetScalarParameterValue("Wet Alpha", Underwater.WetAlpha);
		MID->SetScalarParameterValue("Wet Hardness", Underwater.WetHardness);
		MID->SetScalarParameterValue("Wet Location", Underwater.WetLocation);
		MID->SetScalarParameterValue("Wet Radius", Underwater.WetRadius);
		MID->SetVectorParameterValue("ExponentialFogColorHigh", Underwater.ExponentialFogColorHigh);
		MID->SetVectorParameterValue("ExponentialFogColorLow", Underwater.ExponentialFogColorLow);
		MID->SetVectorParameterValue("SceneColorDeep", Underwater.SceneColorDeep);
		MID->SetVectorParameterValue("SceneColorShallow", Underwater.SceneColorShallow);
		MID->SetTextureParameterValue("DistortionTexture", Underwater.DistortionTexture);
	}

	UFUNCTION(BlueprintPure, Category="Underwater")
	static void LerpUnderwater(
		const FOceanologyUnderwater& A, 
		const FOceanologyUnderwater& B, 
		const double Alpha, 
		FOceanologyUnderwater& OutResult
	)
	{
		FOceanologyUnderwater LocalResult;
		LocalResult.AtmosphereLighting = UKismetMathLibrary::Lerp(A.AtmosphereLighting, B.AtmosphereLighting, Alpha);
		LocalResult.BandOffset = UKismetMathLibrary::Lerp(A.BandOffset, B.BandOffset, Alpha);
		LocalResult.BandOpacity = UKismetMathLibrary::Lerp(A.BandOpacity, B.BandOpacity, Alpha);
		LocalResult.BandWidth = UKismetMathLibrary::Lerp(A.BandWidth, B.BandWidth, Alpha);
		LocalResult.DarknessFadeDepth = UKismetMathLibrary::Lerp(A.DarknessFadeDepth, B.DarknessFadeDepth, Alpha);
		LocalResult.DeepColorDistance = UKismetMathLibrary::Lerp(A.DeepColorDistance, B.DeepColorDistance, Alpha);
		LocalResult.DepthColorAbsorption = UKismetMathLibrary::Lerp(A.DepthColorAbsorption, B.DepthColorAbsorption, Alpha);
		LocalResult.DistortionIntensity = UKismetMathLibrary::Lerp(A.DistortionIntensity, B.DistortionIntensity, Alpha);
		LocalResult.DistortionScale = UKismetMathLibrary::Lerp(A.DistortionScale, B.DistortionScale, Alpha);
		LocalResult.DistortionSpeed = UKismetMathLibrary::Lerp(A.DistortionSpeed, B.DistortionSpeed, Alpha);
		LocalResult.FogDensity = UKismetMathLibrary::Lerp(A.FogDensity, B.FogDensity, Alpha);
		LocalResult.LocalPosition = UKismetMathLibrary::Lerp(A.LocalPosition, B.LocalPosition, Alpha);
		LocalResult.SunPower = UKismetMathLibrary::Lerp(A.SunPower, B.SunPower, Alpha);
		LocalResult.UnderwaterLightDepth = UKismetMathLibrary::Lerp(A.UnderwaterLightDepth, B.UnderwaterLightDepth, Alpha);
		LocalResult.UnderwaterNoLightDepth = UKismetMathLibrary::Lerp(A.UnderwaterNoLightDepth, B.UnderwaterNoLightDepth, Alpha);
		LocalResult.VignetteDark = UKismetMathLibrary::Lerp(A.VignetteDark, B.VignetteDark, Alpha);
		LocalResult.VignetteDistortion = UKismetMathLibrary::Lerp(A.VignetteDistortion, B.VignetteDistortion, Alpha);
		LocalResult.VignetteEdge = UKismetMathLibrary::Lerp(A.VignetteEdge, B.VignetteEdge, Alpha);
		LocalResult.WaterLineColorIntensity = UKismetMathLibrary::Lerp(A.WaterLineColorIntensity, B.WaterLineColorIntensity, Alpha);
		LocalResult.WaterLineDistanceFalloff = UKismetMathLibrary::Lerp(A.WaterLineDistanceFalloff, B.WaterLineDistanceFalloff, Alpha);
		LocalResult.WaterLineRefraction = UKismetMathLibrary::Lerp(A.WaterLineRefraction, B.WaterLineRefraction, Alpha);
		LocalResult.WaterLineRefractionWidth = UKismetMathLibrary::Lerp(A.WaterLineRefractionWidth, B.WaterLineRefractionWidth, Alpha);
		LocalResult.WetAlpha = UKismetMathLibrary::Lerp(A.WetAlpha, B.WetAlpha, Alpha);
		LocalResult.WetHardness = UKismetMathLibrary::Lerp(A.WetHardness, B.WetHardness, Alpha);
		LocalResult.WetLocation = UKismetMathLibrary::Lerp(A.WetLocation, B.WetLocation, Alpha);
		LocalResult.WetRadius = UKismetMathLibrary::Lerp(A.WetRadius, B.WetRadius, Alpha);
		OutResult = LocalResult;
	}
};
