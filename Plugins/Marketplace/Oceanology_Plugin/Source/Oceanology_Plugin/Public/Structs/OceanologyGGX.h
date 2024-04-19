#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyGGX.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyGGX
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GGX", DisplayName="GGX")
	double GGX = 0.02;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GGX", DisplayName="Phong Reflection")
	double PhongReflection = 20.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GGX", DisplayName="Sun Reflection Amount")
	double SunReflectionAmount = 0.995;

	FOceanologyGGX()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyGGXHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="GGX")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyGGX& GGX)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("GGX", GGX.GGX);
		MID->SetScalarParameterValue("Phong Reflection", GGX.PhongReflection);
		MID->SetScalarParameterValue("Sun Reflection Amount", GGX.SunReflectionAmount);
	}

	UFUNCTION(BlueprintPure, Category="GGX")
	static void LerpGGX(
		const FOceanologyGGX& A, 
		const FOceanologyGGX& B, 
		const double Alpha, 
		FOceanologyGGX& OutResult
	)
	{
		FOceanologyGGX LocalResult;
		LocalResult.GGX = UKismetMathLibrary::Lerp(A.GGX, B.GGX, Alpha);
		LocalResult.PhongReflection = UKismetMathLibrary::Lerp(A.PhongReflection, B.PhongReflection, Alpha);
		LocalResult.SunReflectionAmount = UKismetMathLibrary::Lerp(A.SunReflectionAmount, B.SunReflectionAmount, Alpha);
		OutResult = LocalResult;
	}
};
