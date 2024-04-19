#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyGroundCaustics.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyGroundCaustics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundCaustics", DisplayName="DarknessExposure")
	double DarknessExposure = 2.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundCaustics", DisplayName="MaximumDarkness")
	double MaximumDarkness = -4000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundCaustics", DisplayName="MinimumDarkness")
	double MinimumDarkness = -10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundCaustics", DisplayName="Wet Alpha")
	double WetAlpha = 10.0;

	FOceanologyGroundCaustics()
	{
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyGroundCausticsHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="GroundCaustics")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyGroundCaustics& GroundCaustics)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("DarknessExposure", GroundCaustics.DarknessExposure);
		MID->SetScalarParameterValue("MaximumDarkness", GroundCaustics.MaximumDarkness);
		MID->SetScalarParameterValue("MinimumDarkness", GroundCaustics.MinimumDarkness);
		MID->SetScalarParameterValue("Wet Alpha", GroundCaustics.WetAlpha);
	}

	UFUNCTION(BlueprintPure, Category="GroundCaustics")
	static void LerpGroundCaustics(
		const FOceanologyGroundCaustics& A, 
		const FOceanologyGroundCaustics& B, 
		const double Alpha, 
		FOceanologyGroundCaustics& OutResult
	)
	{
		FOceanologyGroundCaustics LocalResult;
		LocalResult.DarknessExposure = UKismetMathLibrary::Lerp(A.DarknessExposure, B.DarknessExposure, Alpha);
		LocalResult.MaximumDarkness = UKismetMathLibrary::Lerp(A.MaximumDarkness, B.MaximumDarkness, Alpha);
		LocalResult.MinimumDarkness = UKismetMathLibrary::Lerp(A.MinimumDarkness, B.MinimumDarkness, Alpha);
		LocalResult.WetAlpha = UKismetMathLibrary::Lerp(A.WetAlpha, B.WetAlpha, Alpha);
		OutResult = LocalResult;
	}
};
