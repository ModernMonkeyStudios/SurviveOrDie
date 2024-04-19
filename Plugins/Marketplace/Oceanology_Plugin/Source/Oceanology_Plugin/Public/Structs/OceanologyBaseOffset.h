#pragma once

#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "OceanologyBaseOffset.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyBaseOffset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Amplitude Σ")
	double Amplitude = 80.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Steepness Σ")
	double Steepness = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Speed Σ")
	double Speed = 1500.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="WaveLength Σ")
	double WaveLength = 16384.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Direction Σ")
	double Direction = 0.4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Direction Offset Σ")
	double Direction_Offset = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves", DisplayName="Direction Offset2 Σ")
	double Direction_Offset2 = 0.75;

	FOceanologyBaseOffset()
	{
	}
};

UCLASS()
class UOceanologyBaseOffsetHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Base Offset")
	static void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanologyBaseOffset& BaseOffset)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Amplitude", BaseOffset.Amplitude);
		MID->SetScalarParameterValue("Direction", BaseOffset.Direction);
		MID->SetScalarParameterValue("Direction_Offset", BaseOffset.Direction_Offset);
		MID->SetScalarParameterValue("Direction_Offset_2", BaseOffset.Direction_Offset2);
		MID->SetScalarParameterValue("Speed", BaseOffset.Speed);
		MID->SetScalarParameterValue("Steepness", BaseOffset.Steepness);
		MID->SetScalarParameterValue("Wavelength", BaseOffset.WaveLength);
	}

	UFUNCTION(BlueprintCallable, Category="Base Offset")
	static void SetMaterialParameterCollections(
		UMaterialParameterCollectionInstance* MPCI,
		const FOceanologyBaseOffset& BaseOffset
	)
	{
		if (MPCI == nullptr)
		{
			return;
		}

		MPCI->SetScalarParameterValue("Amplitude", BaseOffset.Amplitude);
		MPCI->SetScalarParameterValue("Steepness", BaseOffset.Steepness);
		MPCI->SetScalarParameterValue("Speed", BaseOffset.Speed);
		MPCI->SetScalarParameterValue("Wavelength", BaseOffset.WaveLength);
		MPCI->SetScalarParameterValue("Direction", BaseOffset.Direction);
		MPCI->SetScalarParameterValue("Direction_Offset", BaseOffset.Direction_Offset);
		MPCI->SetScalarParameterValue("Direction_Offset_2", BaseOffset.Direction_Offset2);
	}

	UFUNCTION(BlueprintCallable, Category="Base Offset")
	static void SetNiagaraVariables(UNiagaraComponent* Niagara, const FOceanologyBaseOffset& BaseOffset)
	{
		if (Niagara == nullptr)
		{
			return;
		}

		Niagara->SetVariableFloat("Amplitude", BaseOffset.Amplitude);
		Niagara->SetVariableFloat("Direction", BaseOffset.Direction);
		Niagara->SetVariableFloat("Direction_Offset", BaseOffset.Direction_Offset);
		Niagara->SetVariableFloat("Direction_Offset_2", BaseOffset.Direction_Offset2);
		Niagara->SetVariableFloat("Speed", BaseOffset.Speed);
		Niagara->SetVariableFloat("Steepness", BaseOffset.Steepness);
		Niagara->SetVariableFloat("Wavelength", BaseOffset.WaveLength);
	}

	UFUNCTION(BlueprintPure, Category="BaseOffset")
	static void LerpBaseOffset(
		const FOceanologyBaseOffset& A,
		const FOceanologyBaseOffset& B,
		const double Alpha,
		FOceanologyBaseOffset& OutResult
	)
	{
		FOceanologyBaseOffset LocalResult;
		LocalResult.Amplitude = UKismetMathLibrary::Lerp(A.Amplitude, B.Amplitude, Alpha);
		LocalResult.Direction = UKismetMathLibrary::Lerp(A.Direction, B.Direction, Alpha);
		LocalResult.Direction_Offset = UKismetMathLibrary::Lerp(A.Direction_Offset, B.Direction_Offset, Alpha);
		LocalResult.Direction_Offset2 = UKismetMathLibrary::Lerp(A.Direction_Offset2, B.Direction_Offset2, Alpha);
		LocalResult.Speed = UKismetMathLibrary::Lerp(A.Speed, B.Speed, Alpha);
		LocalResult.Steepness = UKismetMathLibrary::Lerp(A.Steepness, B.Steepness, Alpha);
		LocalResult.WaveLength = UKismetMathLibrary::Lerp(A.WaveLength, B.WaveLength, Alpha);
		OutResult = LocalResult;
	}
};
