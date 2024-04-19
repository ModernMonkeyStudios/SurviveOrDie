#pragma once

#include "Engine/Texture.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OceanologyCaustics.generated.h"

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
USTRUCT(BlueprintType)
struct FOceanologyCaustics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="CausticsBrightness")
	double CausticsBrightness = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="CausticsFadeInDistance")
	double CausticsFadeInDistance = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="CausticsScale")
	double CausticsScale = 16.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="FadeDistance")
	double FadeDistance = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="LightParallax")
	double LightParallax = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="MultiplyRefraction")
	double MultiplyRefraction = 1.333;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caustics", DisplayName="T_Caustics")
	UTexture* T_Caustics = nullptr;

	FOceanologyCaustics()
	{
		T_Caustics = LoadObject<UTexture>(nullptr, TEXT("/Oceanology_Plugin/Design/Ocean/Textures/T_Caustics.T_Caustics"), nullptr, LOAD_None, nullptr);
	}
};

/**
 * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!
 * Generated at: 10 March 2024
 **/
UCLASS()
class UOceanologyCausticsHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Caustics")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyCaustics& Caustics)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("CausticsBrightness", Caustics.CausticsBrightness);
		MID->SetScalarParameterValue("CausticsFadeInDistance", Caustics.CausticsFadeInDistance);
		MID->SetScalarParameterValue("CausticsScale", Caustics.CausticsScale);
		MID->SetScalarParameterValue("FadeDistance", Caustics.FadeDistance);
		MID->SetScalarParameterValue("LightParallax", Caustics.LightParallax);
		MID->SetScalarParameterValue("MultiplyRefraction", Caustics.MultiplyRefraction);
		MID->SetTextureParameterValue("T_Caustics", Caustics.T_Caustics);
	}

	UFUNCTION(BlueprintPure, Category="Caustics")
	static void LerpCaustics(
		const FOceanologyCaustics& A, 
		const FOceanologyCaustics& B, 
		const double Alpha, 
		FOceanologyCaustics& OutResult
	)
	{
		FOceanologyCaustics LocalResult;
		LocalResult.CausticsBrightness = UKismetMathLibrary::Lerp(A.CausticsBrightness, B.CausticsBrightness, Alpha);
		LocalResult.CausticsFadeInDistance = UKismetMathLibrary::Lerp(A.CausticsFadeInDistance, B.CausticsFadeInDistance, Alpha);
		LocalResult.CausticsScale = UKismetMathLibrary::Lerp(A.CausticsScale, B.CausticsScale, Alpha);
		LocalResult.FadeDistance = UKismetMathLibrary::Lerp(A.FadeDistance, B.FadeDistance, Alpha);
		LocalResult.LightParallax = UKismetMathLibrary::Lerp(A.LightParallax, B.LightParallax, Alpha);
		LocalResult.MultiplyRefraction = UKismetMathLibrary::Lerp(A.MultiplyRefraction, B.MultiplyRefraction, Alpha);
		OutResult = LocalResult;
	}
};
