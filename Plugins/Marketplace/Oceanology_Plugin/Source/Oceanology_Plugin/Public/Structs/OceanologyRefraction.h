#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyRefraction.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyRefraction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", DisplayName="CriticalAngleBendWidth")
	double CriticalAngleBendWidth = -0.75;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", DisplayName="CriticalAngleDot")
	double CriticalAngleDot = 0.225;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", DisplayName="CriticalAngleWidth")
	double CriticalAngleWidth = 0.01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", DisplayName="LipRefraction")
	double LipRefraction = 0.95;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", DisplayName="RefractionBottomAmount")
	double RefractionBottomAmount = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", DisplayName="RefractionDownsampleFactor")
	double RefractionDownsampleFactor = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Refraction", meta = (ClampMax = "2.0"), DisplayName="RefractionNear")
	double RefractionNear = 1.333;

	FOceanologyRefraction()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyRefractionHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Refraction")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyRefraction& Refraction)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("CriticalAngleBendWidth", Refraction.CriticalAngleBendWidth);
		MID->SetScalarParameterValue("CriticalAngleDot", Refraction.CriticalAngleDot);
		MID->SetScalarParameterValue("CriticalAngleWidth", Refraction.CriticalAngleWidth);
		MID->SetScalarParameterValue("LipRefraction", Refraction.LipRefraction);
		MID->SetScalarParameterValue("RefractionBottomAmount", Refraction.RefractionBottomAmount);
		MID->SetScalarParameterValue("RefractionDownsampleFactor", Refraction.RefractionDownsampleFactor);
		MID->SetScalarParameterValue("RefractionNear", Refraction.RefractionNear);
	}

	UFUNCTION(BlueprintPure, Category="Refraction")
	static void LerpRefraction(
		const FOceanologyRefraction& A, 
		const FOceanologyRefraction& B, 
		const double Alpha, 
		FOceanologyRefraction& OutResult
	)
	{
		FOceanologyRefraction LocalResult;
		LocalResult.CriticalAngleBendWidth = UKismetMathLibrary::Lerp(A.CriticalAngleBendWidth, B.CriticalAngleBendWidth, Alpha);
		LocalResult.CriticalAngleDot = UKismetMathLibrary::Lerp(A.CriticalAngleDot, B.CriticalAngleDot, Alpha);
		LocalResult.CriticalAngleWidth = UKismetMathLibrary::Lerp(A.CriticalAngleWidth, B.CriticalAngleWidth, Alpha);
		LocalResult.LipRefraction = UKismetMathLibrary::Lerp(A.LipRefraction, B.LipRefraction, Alpha);
		LocalResult.RefractionBottomAmount = UKismetMathLibrary::Lerp(A.RefractionBottomAmount, B.RefractionBottomAmount, Alpha);
		LocalResult.RefractionDownsampleFactor = UKismetMathLibrary::Lerp(A.RefractionDownsampleFactor, B.RefractionDownsampleFactor, Alpha);
		LocalResult.RefractionNear = UKismetMathLibrary::Lerp(A.RefractionNear, B.RefractionNear, Alpha);
		OutResult = LocalResult;
	}
};
