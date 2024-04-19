#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyProcedural.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyProcedural
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural", DisplayName="NoiseTiling")
	double NoiseTiling = 128.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Procedural", DisplayName="Turbulance")
	double Turbulance = 1.0;

	FOceanologyProcedural()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyProceduralHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Procedural")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyProcedural& Procedural)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("NoiseTiling", Procedural.NoiseTiling);
		MID->SetScalarParameterValue("Turbulance", Procedural.Turbulance);
	}

	UFUNCTION(BlueprintPure, Category="Procedural")
	static void LerpProcedural(
		const FOceanologyProcedural& A, 
		const FOceanologyProcedural& B, 
		const double Alpha, 
		FOceanologyProcedural& OutResult
	)
	{
		FOceanologyProcedural LocalResult;
		LocalResult.NoiseTiling = UKismetMathLibrary::Lerp(A.NoiseTiling, B.NoiseTiling, Alpha);
		LocalResult.Turbulance = UKismetMathLibrary::Lerp(A.Turbulance, B.Turbulance, Alpha);
		OutResult = LocalResult;
	}
};
