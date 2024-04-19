// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Actors/OceanologyInfiniteOceanActor.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetStringLibrary.h"
#include "OceanologyMaterialToStructConverterActor.generated.h"

UENUM(BlueprintType)
enum EOceanMaterialParameterType
{
	None,
	StaticSwitch,
	Vector,
	Scalar,
	Texture,
	// RVT_Texture
};

USTRUCT(BlueprintType)
struct FOceanMIDFile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	FName FileName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	UMaterialInstanceDynamic* MID = nullptr;

	FOceanMIDFile(FName InFileName, UMaterialInstanceDynamic* InMID)
	{
		FileName = InFileName;
		MID = InMID;
	}

	FOceanMIDFile()
	{
	}
};

USTRUCT(BlueprintType)
struct FOceanMaterialParameter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	FName Group = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	FName Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	TEnumAsByte<EOceanMaterialParameterType> Type = None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	FLinearColor VectorValue = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	float ScalarValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	bool SwitchValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	UTexture* TextureValue = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	URuntimeVirtualTexture* RVT_TextureValue = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	FString Desc = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	float ScalarMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Material To Struct Converter")
	float ScalarMax = 0;

	FOceanMaterialParameter()
	{
	}

	FOceanMaterialParameter(
		const FName& InGroup,
		const FScalarParameterValue& InScalarParameterValue,
		const FString& InDesc,
		const float InScalarMin,
		const float InScalarMax
	)
	{
		Group = InGroup;
		Name = InScalarParameterValue.ParameterInfo.Name;
		Type = Scalar;
		ScalarValue = InScalarParameterValue.ParameterValue;
		Desc = InDesc;
		ScalarMin = InScalarMin;
		ScalarMax = InScalarMax;
	}

	FOceanMaterialParameter(
		const FName& InGroup,
		const FVectorParameterValue& InVectorParameterValue,
		const FString& InDesc
	)
	{
		Group = InGroup;
		Name = InVectorParameterValue.ParameterInfo.Name;
		Type = Vector;
		VectorValue = InVectorParameterValue.ParameterValue;
		Desc = InDesc;
	}

	FOceanMaterialParameter(
		const FName& InGroup,
		const FStaticSwitchParameter& InStaticSwitchParameter,
		const FString& InDesc
	)
	{
		Group = InGroup;
		Name = InStaticSwitchParameter.ParameterInfo.Name;
		Type = StaticSwitch;
		SwitchValue = InStaticSwitchParameter.Value;
		Desc = InDesc;
	}

	FOceanMaterialParameter(
		const FName& InGroup,
		const FTextureParameterValue& InTextureParameterValue,
		const FString& InDesc
	)
	{
		Group = InGroup;
		Name = InTextureParameterValue.ParameterInfo.Name;
		Type = Texture;
		TextureValue = InTextureParameterValue.ParameterValue;
		Desc = InDesc;
	}

	// FOceanMaterialParameter(const FName& InGroup,
	//                         const FRuntimeVirtualTextureParameterValue& InRuntimeVirtualTextureParameterValue)
	// {
	// 	Group = InGroup;
	// 	Name = InRuntimeVirtualTextureParameterValue.ParameterInfo.Name;
	// 	Type = RVT_Texture;
	// 	RVT_TextureValue = InRuntimeVirtualTextureParameterValue.ParameterValue;
	// }

	FString GetValueAsString() const
	{
		switch (Type)
		{
		case None:
			return "";
		case Vector:
			return "FLinearColor("
				+ FString::SanitizeFloat(VectorValue.R)
				+ ", " + FString::SanitizeFloat(VectorValue.G)
				+ ", " + FString::SanitizeFloat(VectorValue.B)
				+ ", " + FString::SanitizeFloat(VectorValue.A)
				+ ")";
		case Scalar:
			return FString::SanitizeFloat(ScalarValue);
		case StaticSwitch:
			return UKismetStringLibrary::Conv_BoolToString(SwitchValue);
		case Texture:
			// case RVT_Texture:
			return "nullptr";
		default: ;
			return "";
		}
	}

	FString GetTypeNameAsString() const
	{
		switch (Type)
		{
		case None:
			return "";
		case Vector:
			return "FLinearColor";
		case Scalar:
			return "double";
		case StaticSwitch:
			return "bool";
		case Texture:
			return "UTexture*";
		// case RVT_Texture:
		// 	return "URuntimeVirtualTexture*";
		default: ;
			return "";
		}
	}
};

UCLASS()
class OCEANOLOGY_PLUGINEDITOR_API AOceanologyMaterialToStructConverter : public AActor
{
	GENERATED_BODY()

public:
	AOceanologyMaterialToStructConverter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	AOceanologyWaterParent* Water = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	TArray<FName> GroupsToInclude = {
		"SurfaceScattering",
		"Caustics",
		"GroundCaustics",
		"Refraction",
		"HorizonCorrection",
		"Flipbook",
		"Foam",
		"Folding",
		"Procedural",
		"WaterProjection",
		"RVT",
		"Underwater",
		"VolumetricFog",
		"Wetness",
		"Bubbles",
		"Mask",
		"MaskUnderwater",
		"Actor Height",
		"GGX"
	};

	void GetAllMIDsForConverter(TArray<FOceanMIDFile>& MIDs) const;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Default")
	void DumpMaterialParametersToStructs();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	FString StructsSourceCodeDirectory = "";
	
	void DumpMaterialParametersToStructs(
		const FName FileName,
		UMaterialInstance* InMaterialInstance
	) const;

	static void ReadMaterialParameters(
		UMaterialInstance* InMaterialInstance,
		TArray<FOceanMaterialParameter>& OutMaterialParameters
	);

	void MapMaterialParameters(
		TArray<FOceanMaterialParameter> InMaterialParameters,
		TMap<FName, TArray<FOceanMaterialParameter>>& OutMappedParameters
	) const;

	static void MapLines(
		TMap<FName, TArray<FOceanMaterialParameter>> InMappedParameters,
		TMap<FName, FString>& OutMappedLines
	);

	static void MapLinesForStruct(
		const TTuple<FName, TArray<FOceanMaterialParameter>>& MappedParameter,
		const FName Group,
		const FString& SanitizedGroupName,
		TMap<FName, FString>& OutMappedLines
	);

	static void MapLinesForHelper(
		const TTuple<FName, TArray<FOceanMaterialParameter>>& MappedParameter,
		const FName Group,
		const FString& SanitizedGroupName,
		TMap<FName, FString>& OutMappedLines
	);

	static void MapLinesForHeaderComment(const FName Group, TMap<FName, FString>& OutMappedLines);

	void WriteOutputFiles(
		const FName FileName,
		TMap<FName, FString> InMappedLines
	) const;

	static void GetMinMaxMetadataString(
		const FOceanMaterialParameter& MaterialParameter,
		FString& MinMaxMetadata
	);
	static FString GetSanitizedGroupName(const FName Group);
	static FString GetSanitizedParameterName(const FName Name);
};
