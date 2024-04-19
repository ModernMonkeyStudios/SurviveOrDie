#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "OceanologyWaves.generated.h"

USTRUCT(BlueprintType)
struct FOceanologyWave_1
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Amplitude Σ1")
	double Amplitude_1 = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Steepness Σ1")
	double Steepness_1 = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Speed Σ1")
	double Speed_1 = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="WaveLength Σ1")
	double WaveLength_1 = 8192.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Direction Σ1")
	double Direction_1 = 0.425;

	FOceanologyWave_1()
	{
	}
};

USTRUCT(BlueprintType)
struct FOceanologyWave_2
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Amplitude Σ2")
	double Amplitude_2 = 20.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Steepness Σ2")
	double Steepness_2 = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Speed Σ2")
	double Speed_2 = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="WaveLength Σ2")
	double WaveLength_2 = 4096.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Direction Σ2")
	double Direction_2 = 0.45;

	FOceanologyWave_2()
	{
	}
};

USTRUCT(BlueprintType)
struct FOceanologyWave_3
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Amplitude Σ3")
	double Amplitude_3 = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Steepness Σ3")
	double Steepness_3 = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Speed Σ3")
	double Speed_3 = 500.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="WaveLength Σ3")
	double WaveLength_3 = 2048.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Direction Σ3")
	double Direction_3 = 0.475;

	FOceanologyWave_3()
	{
	}
};

USTRUCT(BlueprintType)
struct FOceanologyWave_4
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Amplitude Σ4")
	double Amplitude_4 = 5.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Steepness Σ4")
	double Steepness_4 = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Speed Σ4")
	double Speed_4 = 250.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="WaveLength Σ4")
	double WaveLength_4 = 1024.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", DisplayName="Direction Σ4")
	double Direction_4 = 0.5;

	FOceanologyWave_4()
	{
	}
};


UCLASS()
class UOceanologyWavesHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Wave 1")
	static void SetMaterialParametersWave_1(UMaterialInstanceDynamic* MID, const FOceanologyWave_1& Wave_1)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Amplitude_1", Wave_1.Amplitude_1);
		MID->SetScalarParameterValue("Direction_1", Wave_1.Direction_1);
		MID->SetScalarParameterValue("Speed_1", Wave_1.Speed_1);
		MID->SetScalarParameterValue("Steepness_1", Wave_1.Steepness_1);
		MID->SetScalarParameterValue("Wavelength_1", Wave_1.WaveLength_1);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 1")
	static void SetMaterialParameterCollectionsWave_1(UMaterialParameterCollectionInstance* MPCI, const FOceanologyWave_1& Wave_1)
	{
		if (MPCI == nullptr)
		{
			return;
		}

		MPCI->SetScalarParameterValue("Amplitude_1", Wave_1.Amplitude_1);
		MPCI->SetScalarParameterValue("Direction_1", Wave_1.Direction_1);
		MPCI->SetScalarParameterValue("Speed_1", Wave_1.Speed_1);
		MPCI->SetScalarParameterValue("Steepness_1", Wave_1.Steepness_1);
		MPCI->SetScalarParameterValue("Wavelength_1", Wave_1.WaveLength_1);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 1")
	static void SetNiagaraVariablesWave_1(UNiagaraComponent* Niagara, const FOceanologyWave_1& Wave_1)
	{
		if (Niagara == nullptr)
		{
			return;
		}

		Niagara->SetVariableFloat("Amplitude_1", Wave_1.Amplitude_1);
		Niagara->SetVariableFloat("Direction_1", Wave_1.Direction_1);
		Niagara->SetVariableFloat("Speed_1", Wave_1.Speed_1);
		Niagara->SetVariableFloat("Steepness_1", Wave_1.Steepness_1);
		Niagara->SetVariableFloat("Wavelength_1", Wave_1.WaveLength_1);
	}

	UFUNCTION(BlueprintPure, Category="Wave 1")
	static void LerpWave_1(
		const FOceanologyWave_1& A,
		const FOceanologyWave_1& B,
		const double Alpha,
		FOceanologyWave_1& OutResult
	)
	{
		FOceanologyWave_1 LocalResult;

		LocalResult.Amplitude_1 = UKismetMathLibrary::Lerp(A.Amplitude_1, B.Amplitude_1, Alpha);
		LocalResult.Steepness_1 = UKismetMathLibrary::Lerp(A.Steepness_1, B.Steepness_1, Alpha);
		LocalResult.Speed_1 = UKismetMathLibrary::Lerp(A.Speed_1, B.Speed_1, Alpha);
		LocalResult.WaveLength_1 = UKismetMathLibrary::Lerp(A.WaveLength_1, B.WaveLength_1, Alpha);
		LocalResult.Direction_1 = UKismetMathLibrary::Lerp(A.Direction_1, B.Direction_1, Alpha);

		OutResult = LocalResult;
	}

	UFUNCTION(BlueprintCallable, Category="Wave 2")
	static void SetMaterialParametersWave_2(UMaterialInstanceDynamic* MID, const FOceanologyWave_2& Wave_2)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Amplitude_2", Wave_2.Amplitude_2);
		MID->SetScalarParameterValue("Direction_2", Wave_2.Direction_2);
		MID->SetScalarParameterValue("Speed_2", Wave_2.Speed_2);
		MID->SetScalarParameterValue("Steepness_2", Wave_2.Steepness_2);
		MID->SetScalarParameterValue("Wavelength_2", Wave_2.WaveLength_2);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 2")
	static void SetMaterialParameterCollectionsWave_2(UMaterialParameterCollectionInstance* MPCI, const FOceanologyWave_2& Wave_2)
	{
		if (MPCI == nullptr)
		{
			return;
		}

		MPCI->SetScalarParameterValue("Amplitude_2", Wave_2.Amplitude_2);
		MPCI->SetScalarParameterValue("Direction_2", Wave_2.Direction_2);
		MPCI->SetScalarParameterValue("Speed_2", Wave_2.Speed_2);
		MPCI->SetScalarParameterValue("Steepness_2", Wave_2.Steepness_2);
		MPCI->SetScalarParameterValue("Wavelength_2", Wave_2.WaveLength_2);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 2")
	static void SetNiagaraVariablesWave_2(UNiagaraComponent* Niagara, const FOceanologyWave_2& Wave_2)
	{
		if (Niagara == nullptr)
		{
			return;
		}

		Niagara->SetVariableFloat("Amplitude_2", Wave_2.Amplitude_2);
		Niagara->SetVariableFloat("Direction_2", Wave_2.Direction_2);
		Niagara->SetVariableFloat("Speed_2", Wave_2.Speed_2);
		Niagara->SetVariableFloat("Steepness_2", Wave_2.Steepness_2);
		Niagara->SetVariableFloat("Wavelength_2", Wave_2.WaveLength_2);
	}

	UFUNCTION(BlueprintPure, Category="Wave 2")
	static void LerpWave_2(
		const FOceanologyWave_2& A,
		const FOceanologyWave_2& B,
		const double Alpha,
		FOceanologyWave_2& OutResult
	)
	{
		FOceanologyWave_2 LocalResult;

		LocalResult.Amplitude_2 = UKismetMathLibrary::Lerp(A.Amplitude_2, B.Amplitude_2, Alpha);
		LocalResult.Steepness_2 = UKismetMathLibrary::Lerp(A.Steepness_2, B.Steepness_2, Alpha);
		LocalResult.Speed_2 = UKismetMathLibrary::Lerp(A.Speed_2, B.Speed_2, Alpha);
		LocalResult.WaveLength_2 = UKismetMathLibrary::Lerp(A.WaveLength_2, B.WaveLength_2, Alpha);
		LocalResult.Direction_2 = UKismetMathLibrary::Lerp(A.Direction_2, B.Direction_2, Alpha);

		OutResult = LocalResult;
	}

	UFUNCTION(BlueprintCallable, Category="Wave 3")
	static void SetMaterialParametersWave_3(UMaterialInstanceDynamic* MID, const FOceanologyWave_3& Wave_3)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Amplitude_3", Wave_3.Amplitude_3);
		MID->SetScalarParameterValue("Direction_3", Wave_3.Direction_3);
		MID->SetScalarParameterValue("Speed_3", Wave_3.Speed_3);
		MID->SetScalarParameterValue("Steepness_3", Wave_3.Steepness_3);
		MID->SetScalarParameterValue("Wavelength_3", Wave_3.WaveLength_3);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 3")
	static void SetMaterialParameterCollectionsWave_3(UMaterialParameterCollectionInstance* MPCI, const FOceanologyWave_3& Wave_3)
	{
		if (MPCI == nullptr)
		{
			return;
		}

		MPCI->SetScalarParameterValue("Amplitude_3", Wave_3.Amplitude_3);
		MPCI->SetScalarParameterValue("Direction_3", Wave_3.Direction_3);
		MPCI->SetScalarParameterValue("Speed_3", Wave_3.Speed_3);
		MPCI->SetScalarParameterValue("Steepness_3", Wave_3.Steepness_3);
		MPCI->SetScalarParameterValue("Wavelength_3", Wave_3.WaveLength_3);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 3")
	static void SetNiagaraVariablesWave_3(UNiagaraComponent* Niagara, const FOceanologyWave_3& Wave_3)
	{
		if (Niagara == nullptr)
		{
			return;
		}

		Niagara->SetVariableFloat("Amplitude_3", Wave_3.Amplitude_3);
		Niagara->SetVariableFloat("Direction_3", Wave_3.Direction_3);
		Niagara->SetVariableFloat("Speed_3", Wave_3.Speed_3);
		Niagara->SetVariableFloat("Steepness_3", Wave_3.Steepness_3);
		Niagara->SetVariableFloat("Wavelength_3", Wave_3.WaveLength_3);
	}

	UFUNCTION(BlueprintPure, Category="Wave 3")
	static void LerpWave_3(
		const FOceanologyWave_3& A,
		const FOceanologyWave_3& B,
		const double Alpha,
		FOceanologyWave_3& OutResult
	)
	{
		FOceanologyWave_3 LocalResult;

		LocalResult.Amplitude_3 = UKismetMathLibrary::Lerp(A.Amplitude_3, B.Amplitude_3, Alpha);
		LocalResult.Steepness_3 = UKismetMathLibrary::Lerp(A.Steepness_3, B.Steepness_3, Alpha);
		LocalResult.Speed_3 = UKismetMathLibrary::Lerp(A.Speed_3, B.Speed_3, Alpha);
		LocalResult.WaveLength_3 = UKismetMathLibrary::Lerp(A.WaveLength_3, B.WaveLength_3, Alpha);
		LocalResult.Direction_3 = UKismetMathLibrary::Lerp(A.Direction_3, B.Direction_3, Alpha);

		OutResult = LocalResult;
	}


	UFUNCTION(BlueprintCallable, Category="Wave 4")
	static void SetMaterialParametersWave_4(UMaterialInstanceDynamic* MID, const FOceanologyWave_4& Wave_4)
	{
		if (MID == nullptr)
		{
			return;
		}

		MID->SetScalarParameterValue("Amplitude_4", Wave_4.Amplitude_4);
		MID->SetScalarParameterValue("Direction_4", Wave_4.Direction_4);
		MID->SetScalarParameterValue("Speed_4", Wave_4.Speed_4);
		MID->SetScalarParameterValue("Steepness_4", Wave_4.Steepness_4);
		MID->SetScalarParameterValue("Wavelength_4", Wave_4.WaveLength_4);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 4")
	static void SetMaterialParameterCollectionsWave_4(UMaterialParameterCollectionInstance* MPCI, const FOceanologyWave_4& Wave_4)
	{
		if (MPCI == nullptr)
		{
			return;
		}

		MPCI->SetScalarParameterValue("Amplitude_4", Wave_4.Amplitude_4);
		MPCI->SetScalarParameterValue("Direction_4", Wave_4.Direction_4);
		MPCI->SetScalarParameterValue("Speed_4", Wave_4.Speed_4);
		MPCI->SetScalarParameterValue("Steepness_4", Wave_4.Steepness_4);
		MPCI->SetScalarParameterValue("Wavelength_4", Wave_4.WaveLength_4);
	}

	UFUNCTION(BlueprintCallable, Category="Wave 4")
	static void SetNiagaraVariablesWave_4(UNiagaraComponent* Niagara, const FOceanologyWave_4& Wave_4)
	{
		if (Niagara == nullptr)
		{
			return;
		}

		Niagara->SetVariableFloat("Amplitude_4", Wave_4.Amplitude_4);
		Niagara->SetVariableFloat("Direction_4", Wave_4.Direction_4);
		Niagara->SetVariableFloat("Speed_4", Wave_4.Speed_4);
		Niagara->SetVariableFloat("Steepness_4", Wave_4.Steepness_4);
		Niagara->SetVariableFloat("Wavelength_4", Wave_4.WaveLength_4);
	}

	UFUNCTION(BlueprintPure, Category="Wave 4")
	static void LerpWave_4(
		const FOceanologyWave_4& A,
		const FOceanologyWave_4& B,
		const double Alpha,
		FOceanologyWave_4& OutResult
	)
	{
		FOceanologyWave_4 LocalResult;

		LocalResult.Amplitude_4 = UKismetMathLibrary::Lerp(A.Amplitude_4, B.Amplitude_4, Alpha);
		LocalResult.Steepness_4 = UKismetMathLibrary::Lerp(A.Steepness_4, B.Steepness_4, Alpha);
		LocalResult.Speed_4 = UKismetMathLibrary::Lerp(A.Speed_4, B.Speed_4, Alpha);
		LocalResult.WaveLength_4 = UKismetMathLibrary::Lerp(A.WaveLength_4, B.WaveLength_4, Alpha);
		LocalResult.Direction_4 = UKismetMathLibrary::Lerp(A.Direction_4, B.Direction_4, Alpha);

		OutResult = LocalResult;
	}
};
