#pragma once

#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "OceanologyGlobalDisplacement.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyGlobalDisplacement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Size (M)")
	double SizeM = 8.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Overall Length")
	double OverallLength = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Global Amplitude")
	double GlobalAmplitude = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Global Speed")
	double GlobalSpeed = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Choppiness")
	double Choppiness = 3.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Global Wave Direction")
	double GlobalWaveDirection = 1.0;

	FOceanologyGlobalDisplacement()
	{
	}
};

UCLASS()
class UOceanologyGlobalDisplacementHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Global Displacement")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyGlobalDisplacement& GlobalDisplacement)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("PatchSize", GlobalDisplacement.SizeM);
		MID->SetScalarParameterValue("Choppiness", GlobalDisplacement.Choppiness);
		MID->SetScalarParameterValue("Global Amplitude", GlobalDisplacement.GlobalAmplitude);
		MID->SetScalarParameterValue("Global Speed", GlobalDisplacement.GlobalSpeed);
		MID->SetScalarParameterValue("GlobalWaveDirection", GlobalDisplacement.GlobalWaveDirection);
		MID->SetScalarParameterValue("Overall Length", GlobalDisplacement.OverallLength);
	}

	UFUNCTION(BlueprintCallable, Category="Global Displacement")
	static void SetMaterialParameterCollections(
		UMaterialParameterCollectionInstance* MPCI,
		const FOceanologyGlobalDisplacement& GlobalDisplacement
	)
	{
		if (MPCI == nullptr)
		{
			return;
		}

		MPCI->SetScalarParameterValue("PatchSize", GlobalDisplacement.SizeM);
		MPCI->SetScalarParameterValue("Overall Length", GlobalDisplacement.OverallLength);
		MPCI->SetScalarParameterValue("Global Amplitude", GlobalDisplacement.GlobalAmplitude);
		MPCI->SetScalarParameterValue("Global Speed", GlobalDisplacement.GlobalSpeed);
		MPCI->SetScalarParameterValue("Choppiness", GlobalDisplacement.Choppiness);
		MPCI->SetScalarParameterValue("GlobalWaveDirection", GlobalDisplacement.GlobalWaveDirection);
	}

	UFUNCTION(BlueprintCallable, Category="Global Displacement")
	static void SetNiagaraVariables(UNiagaraComponent* Niagara, const FOceanologyGlobalDisplacement& GlobalDisplacement)
	{
		if (Niagara == nullptr)
		{
			return;
		}

		Niagara->SetVariableFloat("PatchSize", GlobalDisplacement.SizeM);
		Niagara->SetVariableFloat("Choppiness", GlobalDisplacement.Choppiness);
		Niagara->SetVariableFloat("Global Amplitude", GlobalDisplacement.GlobalAmplitude);
		Niagara->SetVariableFloat("Global Speed", GlobalDisplacement.GlobalSpeed);
		Niagara->SetVariableFloat("GlobalWaveDirection", GlobalDisplacement.GlobalWaveDirection);
		Niagara->SetVariableFloat("Overall Length", GlobalDisplacement.OverallLength);
	}

	UFUNCTION(BlueprintPure, Category="GlobalDisplacement")
	static void LerpGlobalDisplacement(
		const FOceanologyGlobalDisplacement& A,
		const FOceanologyGlobalDisplacement& B,
		const double Alpha,
		FOceanologyGlobalDisplacement& OutResult
	)
	{
		FOceanologyGlobalDisplacement LocalResult;
		LocalResult.Choppiness = UKismetMathLibrary::Lerp(A.Choppiness, B.Choppiness, Alpha);
		LocalResult.GlobalAmplitude = UKismetMathLibrary::Lerp(A.GlobalAmplitude, B.GlobalAmplitude, Alpha);
		LocalResult.GlobalSpeed = UKismetMathLibrary::Lerp(A.GlobalSpeed, B.GlobalSpeed, Alpha);
		LocalResult.GlobalWaveDirection = UKismetMathLibrary::Lerp(A.GlobalWaveDirection, B.GlobalWaveDirection, Alpha);
		LocalResult.OverallLength = UKismetMathLibrary::Lerp(A.OverallLength, B.OverallLength, Alpha);
		LocalResult.SizeM = UKismetMathLibrary::Lerp(A.SizeM, B.SizeM, Alpha);
		OutResult = LocalResult;
	}
};
