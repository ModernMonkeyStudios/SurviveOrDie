#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologySurfaceScattering.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologySurfaceScattering
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="CameraVectorPower")
	double CameraVectorPower = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="FadeLengthScattering")
	double FadeLengthScattering = 600000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="FadeOffsetScattering")
	double FadeOffsetScattering = 1000.0;

	/** Single layer water attribute. (HIGH) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", meta = (ClampMin = "-500.0",ClampMax = "1000.0"), DisplayName="PhaseGHigh")
	double PhaseGHigh = 0.4;

	/** Single layer water attribute. (LOW) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", meta = (ClampMin = "-100.0",ClampMax = "500.0"), DisplayName="PhaseGLow")
	double PhaseGLow = 0.75;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="ScatterBoost")
	double ScatterBoost = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="SurfaceScatteringExponential")
	double SurfaceScatteringExponential = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="SurfaceScatteringIntensity")
	double SurfaceScatteringIntensity = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="SurfaceScatteringPower")
	double SurfaceScatteringPower = 5.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="SurfaceScatteringRefractMax")
	double SurfaceScatteringRefractMax = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="SurfaceScatteringRefractMin")
	double SurfaceScatteringRefractMin = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="TopDownExp")
	double TopDownExp = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Water Fresnel  Exponenth")
	double WaterFresnelExponenth = 20.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Water Fresnel Roughness")
	double WaterFresnelRoughness = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Water Fresnel Specular")
	double WaterFresnelSpecular = 0.225;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Water Roughness")
	double WaterRoughness = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Water Specular")
	double WaterSpecular = 0.1125;

	/** This is the surface absorption color of the water. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Absorption")
	FLinearColor Absorption = FLinearColor(20.0, 30.0, 25.0, 25.0);

	/** This is the surface far absorption color of the water. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Absorption Far")
	FLinearColor AbsorptionFar = FLinearColor(10.0, 135.0, 200.0, 35.0);

	/** This is the surface scattering color of the water. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Scattering")
	FLinearColor Scattering = FLinearColor(0.003677, 0.124772, 0.223228, 1.0);

	/** This is the far surface scattering color of the water. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SurfaceScattering", DisplayName="Scattering Far")
	FLinearColor ScatteringFar = FLinearColor(0.0, 0.25, 0.3, 0.05);

	FOceanologySurfaceScattering()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologySurfaceScatteringHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SurfaceScattering")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologySurfaceScattering& SurfaceScattering)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("CameraVectorPower", SurfaceScattering.CameraVectorPower);
		MID->SetScalarParameterValue("FadeLengthScattering", SurfaceScattering.FadeLengthScattering);
		MID->SetScalarParameterValue("FadeOffsetScattering", SurfaceScattering.FadeOffsetScattering);
		MID->SetScalarParameterValue("PhaseGHigh", SurfaceScattering.PhaseGHigh);
		MID->SetScalarParameterValue("PhaseGLow", SurfaceScattering.PhaseGLow);
		MID->SetScalarParameterValue("ScatterBoost", SurfaceScattering.ScatterBoost);
		MID->SetScalarParameterValue("SurfaceScatteringExponential", SurfaceScattering.SurfaceScatteringExponential);
		MID->SetScalarParameterValue("SurfaceScatteringIntensity", SurfaceScattering.SurfaceScatteringIntensity);
		MID->SetScalarParameterValue("SurfaceScatteringPower", SurfaceScattering.SurfaceScatteringPower);
		MID->SetScalarParameterValue("SurfaceScatteringRefractMax", SurfaceScattering.SurfaceScatteringRefractMax);
		MID->SetScalarParameterValue("SurfaceScatteringRefractMin", SurfaceScattering.SurfaceScatteringRefractMin);
		MID->SetScalarParameterValue("TopDownExp", SurfaceScattering.TopDownExp);
		MID->SetScalarParameterValue("Water Fresnel  Exponenth", SurfaceScattering.WaterFresnelExponenth);
		MID->SetScalarParameterValue("Water Fresnel Roughness", SurfaceScattering.WaterFresnelRoughness);
		MID->SetScalarParameterValue("Water Fresnel Specular", SurfaceScattering.WaterFresnelSpecular);
		MID->SetScalarParameterValue("Water Roughness", SurfaceScattering.WaterRoughness);
		MID->SetScalarParameterValue("Water Specular", SurfaceScattering.WaterSpecular);
		MID->SetVectorParameterValue("Absorption", SurfaceScattering.Absorption);
		MID->SetVectorParameterValue("Absorption Far", SurfaceScattering.AbsorptionFar);
		MID->SetVectorParameterValue("Scattering", SurfaceScattering.Scattering);
		MID->SetVectorParameterValue("Scattering Far", SurfaceScattering.ScatteringFar);
	}

	UFUNCTION(BlueprintPure, Category="SurfaceScattering")
	static void LerpSurfaceScattering(
		const FOceanologySurfaceScattering& A, 
		const FOceanologySurfaceScattering& B, 
		const double Alpha, 
		FOceanologySurfaceScattering& OutResult
	)
	{
		FOceanologySurfaceScattering LocalResult;
		LocalResult.CameraVectorPower = UKismetMathLibrary::Lerp(A.CameraVectorPower, B.CameraVectorPower, Alpha);
		LocalResult.FadeLengthScattering = UKismetMathLibrary::Lerp(A.FadeLengthScattering, B.FadeLengthScattering, Alpha);
		LocalResult.FadeOffsetScattering = UKismetMathLibrary::Lerp(A.FadeOffsetScattering, B.FadeOffsetScattering, Alpha);
		LocalResult.PhaseGHigh = UKismetMathLibrary::Lerp(A.PhaseGHigh, B.PhaseGHigh, Alpha);
		LocalResult.PhaseGLow = UKismetMathLibrary::Lerp(A.PhaseGLow, B.PhaseGLow, Alpha);
		LocalResult.ScatterBoost = UKismetMathLibrary::Lerp(A.ScatterBoost, B.ScatterBoost, Alpha);
		LocalResult.SurfaceScatteringExponential = UKismetMathLibrary::Lerp(A.SurfaceScatteringExponential, B.SurfaceScatteringExponential, Alpha);
		LocalResult.SurfaceScatteringIntensity = UKismetMathLibrary::Lerp(A.SurfaceScatteringIntensity, B.SurfaceScatteringIntensity, Alpha);
		LocalResult.SurfaceScatteringPower = UKismetMathLibrary::Lerp(A.SurfaceScatteringPower, B.SurfaceScatteringPower, Alpha);
		LocalResult.SurfaceScatteringRefractMax = UKismetMathLibrary::Lerp(A.SurfaceScatteringRefractMax, B.SurfaceScatteringRefractMax, Alpha);
		LocalResult.SurfaceScatteringRefractMin = UKismetMathLibrary::Lerp(A.SurfaceScatteringRefractMin, B.SurfaceScatteringRefractMin, Alpha);
		LocalResult.TopDownExp = UKismetMathLibrary::Lerp(A.TopDownExp, B.TopDownExp, Alpha);
		LocalResult.WaterFresnelExponenth = UKismetMathLibrary::Lerp(A.WaterFresnelExponenth, B.WaterFresnelExponenth, Alpha);
		LocalResult.WaterFresnelRoughness = UKismetMathLibrary::Lerp(A.WaterFresnelRoughness, B.WaterFresnelRoughness, Alpha);
		LocalResult.WaterFresnelSpecular = UKismetMathLibrary::Lerp(A.WaterFresnelSpecular, B.WaterFresnelSpecular, Alpha);
		LocalResult.WaterRoughness = UKismetMathLibrary::Lerp(A.WaterRoughness, B.WaterRoughness, Alpha);
		LocalResult.WaterSpecular = UKismetMathLibrary::Lerp(A.WaterSpecular, B.WaterSpecular, Alpha);
		OutResult = LocalResult;
	}
};
