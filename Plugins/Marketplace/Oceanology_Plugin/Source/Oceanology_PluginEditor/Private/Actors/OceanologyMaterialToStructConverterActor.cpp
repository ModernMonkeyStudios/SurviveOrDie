// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#include "Actors/OceanologyMaterialToStructConverterActor.h"
#include "Actors/OceanologyLakeActor.h"
#include "Components/OceanologyUnderwaterComponent.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/GameplayStatics.h"
#include "VT/RuntimeVirtualTexture.h"

AOceanologyMaterialToStructConverter::AOceanologyMaterialToStructConverter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOceanologyMaterialToStructConverter::GetAllMIDsForConverter(TArray<FOceanMIDFile>& MIDs) const
{
	if (Water->UnderwaterComponent)
	{
		MIDs.Add(FOceanMIDFile("Underwater", Water->UnderwaterComponent->GetUnderwaterMID()));
		MIDs.Add(FOceanMIDFile("VolumetricFog", Water->UnderwaterComponent->GetVolumetricFogMID()));
		MIDs.Add(FOceanMIDFile("Bubbles", Water->UnderwaterComponent->GetBubblesMID()));
	}

	if (Water->IsA(AOceanologyInfiniteOcean::StaticClass()))
	{
		MIDs.Add(FOceanMIDFile("Wetness", Cast<AOceanologyInfiniteOcean>(Water)->GetShorelineWetnessMID()));
	}

	if (Water->IsA(AOceanologyLake::StaticClass()))
	{
		MIDs.Add(FOceanMIDFile("GroundCaustics", Cast<AOceanologyLake>(Water)->GetCausticsMID()));
	}

	MIDs.Add(FOceanMIDFile("ANY", Water->GetWaterMID()));
}

void AOceanologyMaterialToStructConverter::DumpMaterialParametersToStructs()
{
	if (Water == nullptr)
	{
		return;
	}

	StructsSourceCodeDirectory = IPluginManager::Get()
	                             .FindPlugin(TEXT("Oceanology_Plugin"))
	                             ->GetBaseDir()
		/ TEXT("Source/Oceanology_Plugin/Public/Structs/");

	TArray<FOceanMIDFile> OceanMIDFiles;
	GetAllMIDsForConverter(OceanMIDFiles);

	for (const FOceanMIDFile OceanMIDFile : OceanMIDFiles)
	{
		if (!OceanMIDFile.MID)
		{
			continue;
		}

		DumpMaterialParametersToStructs(OceanMIDFile.FileName, Cast<UMaterialInstance>(OceanMIDFile.MID->Parent));
	}
}

void AOceanologyMaterialToStructConverter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (Water == nullptr)
	{
		Water = Cast<AOceanologyInfiniteOcean>(
			UGameplayStatics::GetActorOfClass(this, AOceanologyInfiniteOcean::StaticClass())
		);
	}

	if (Water == nullptr)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Water is not available! Parser will not function!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);
	}
}

void AOceanologyMaterialToStructConverter::DumpMaterialParametersToStructs(
	const FName FileName,
	UMaterialInstance* InMaterialInstance
) const
{
	if (InMaterialInstance == nullptr)
	{
		UKismetSystemLibrary::PrintString(
			this,
			"Material Instance is missing!",
			true,
			true,
			FLinearColor::Red,
			1.0
		);
		return;
	}

	TArray<FOceanMaterialParameter> MaterialParameters;
	TMap<FName, TArray<FOceanMaterialParameter>> MappedParameters;
	TMap<FName, FString> MappedLines;

	ReadMaterialParameters(InMaterialInstance, MaterialParameters);
	MapMaterialParameters(MaterialParameters, MappedParameters);
	MapLines(MappedParameters, MappedLines);
	WriteOutputFiles(FileName, MappedLines);
}

void AOceanologyMaterialToStructConverter::ReadMaterialParameters(
	UMaterialInstance* InMaterialInstance,
	TArray<FOceanMaterialParameter>& OutMaterialParameters
)
{
	UMaterial* BaseMaterial = InMaterialInstance->GetBaseMaterial();

	for (FStaticSwitchParameter StaticSwitchParameter : InMaterialInstance->GetStaticParameters().
	                                                                        StaticSwitchParameters)
	{
		FName Group;
		InMaterialInstance->GetGroupName(StaticSwitchParameter.ParameterInfo, Group);

		FString Desc;
		BaseMaterial->GetParameterDesc(FHashedMaterialParameterInfo(StaticSwitchParameter.ParameterInfo), Desc);

		OutMaterialParameters.Add(FOceanMaterialParameter(Group, StaticSwitchParameter, Desc));
	}

	for (FScalarParameterValue ScalarParameterValue : InMaterialInstance->ScalarParameterValues)
	{
		FName Group;
		InMaterialInstance->GetGroupName(ScalarParameterValue.ParameterInfo, Group);

		FString Desc;
		BaseMaterial->GetParameterDesc(FHashedMaterialParameterInfo(ScalarParameterValue.ParameterInfo), Desc);

		float Min = 0;
		float Max = 0;
		BaseMaterial->GetScalarParameterSliderMinMax(
			FHashedMaterialParameterInfo(ScalarParameterValue.ParameterInfo),
			Min,
			Max
		);

		OutMaterialParameters.Add(FOceanMaterialParameter(Group, ScalarParameterValue, Desc, Min, Max));
	}

	for (FVectorParameterValue VectorParameterValue : InMaterialInstance->VectorParameterValues)
	{
		FName Group;
		InMaterialInstance->GetGroupName(VectorParameterValue.ParameterInfo, Group);

		FString Desc;
		BaseMaterial->GetParameterDesc(FHashedMaterialParameterInfo(VectorParameterValue.ParameterInfo), Desc);

		OutMaterialParameters.Add(FOceanMaterialParameter(Group, VectorParameterValue, Desc));
	}

	for (FTextureParameterValue TextureParameterValue : InMaterialInstance->TextureParameterValues)
	{
		FName Group;
		InMaterialInstance->GetGroupName(TextureParameterValue.ParameterInfo, Group);

		FString Desc;
		BaseMaterial->GetParameterDesc(FHashedMaterialParameterInfo(TextureParameterValue.ParameterInfo), Desc);

		OutMaterialParameters.Add(FOceanMaterialParameter(Group, TextureParameterValue, Desc));
	}

	// for (FRuntimeVirtualTextureParameterValue RuntimeVirtualTextureParameterValue : MaterialInstance->
	//      RuntimeVirtualTextureParameterValues)
	// {
	// 	FName Group;
	// 	MaterialInstance->GetGroupName(RuntimeVirtualTextureParameterValue.ParameterInfo, Group);
	//
	// 	MaterialParameters.Add(FOceanMaterialParameter(Group, RuntimeVirtualTextureParameterValue));
	// }
}

void AOceanologyMaterialToStructConverter::MapMaterialParameters(
	TArray<FOceanMaterialParameter> InMaterialParameters,
	TMap<FName, TArray<FOceanMaterialParameter>>& OutMappedParameters
) const
{
	for (FOceanMaterialParameter MaterialParameter : InMaterialParameters)
	{
		if (!GroupsToInclude.Contains(MaterialParameter.Group)
			|| MaterialParameter.Group.IsEqual("None")
			|| MaterialParameter.Group.IsEqual(""))
		{
			continue;
		}

		if (!OutMappedParameters.Contains(MaterialParameter.Group))
		{
			OutMappedParameters.Add(MaterialParameter.Group, {});
		}

		OutMappedParameters[MaterialParameter.Group].Add(MaterialParameter);
	}
}

void AOceanologyMaterialToStructConverter::MapLines(
	TMap<FName, TArray<FOceanMaterialParameter>> InMappedParameters,
	TMap<FName, FString>& OutMappedLines
)
{
	for (const TTuple<FName, TArray<FOceanMaterialParameter>> MappedParameter : InMappedParameters)
	{
		const FName Group = MappedParameter.Key;
		const FString SanitizedGroupName = GetSanitizedGroupName(Group);

		MapLinesForStruct(MappedParameter, Group, SanitizedGroupName, OutMappedLines);
		MapLinesForHelper(MappedParameter, Group, SanitizedGroupName, OutMappedLines);
	}
}

void AOceanologyMaterialToStructConverter::MapLinesForStruct(
	const TTuple<FName, TArray<FOceanMaterialParameter>>& MappedParameter,
	const FName Group,
	const FString& SanitizedGroupName,
	TMap<FName, FString>& OutMappedLines
)
{
	bool IncludeT = false;
	bool IncludeVT = false;
	TArray<FOceanMaterialParameter> LocalMaterialParameters = MappedParameter.Value;
	for (FOceanMaterialParameter MaterialParameter : LocalMaterialParameters)
	{
		if (!OutMappedLines.Contains(MaterialParameter.Group))
		{
			OutMappedLines.Add(MaterialParameter.Group, FString());
		}

		if (MaterialParameter.TextureValue != nullptr)
		{
			IncludeT = true;
		}

		if (MaterialParameter.RVT_TextureValue != nullptr)
		{
			IncludeVT = true;
		}
	}

	OutMappedLines[Group].Append("#pragma once\n\n");
	if (IncludeT)
	{
		OutMappedLines[Group].Append("#include \"Engine/Texture.h\"\n");
	}
	if (IncludeVT)
	{
		OutMappedLines[Group].Append("#include \"VT/RuntimeVirtualTexture.h\"\n");
	}
	OutMappedLines[Group].Append("#include \"Kismet/KismetMathLibrary.h\"\n");
	OutMappedLines[Group].Append("#include \"Materials/MaterialInstanceDynamic.h\"\n");
	OutMappedLines[Group].Append(
		"#include \"Oceanology" + SanitizedGroupName +
		".generated.h\"\n\n"
	);

	MapLinesForHeaderComment(Group, OutMappedLines);

	OutMappedLines[Group].Append(
		"USTRUCT(BlueprintType)\nstruct FOceanology" + SanitizedGroupName +
		"\n{\n\tGENERATED_BODY()\n\n"
	);

	for (FOceanMaterialParameter MaterialParameter : LocalMaterialParameters)
	{
		if (MaterialParameter.Type == StaticSwitch)
		{
			continue;
		}

		if (!MaterialParameter.Desc.IsEmpty())
		{
			OutMappedLines[MaterialParameter.Group].Append("\t/** " + MaterialParameter.Desc + " */\n");
		}

		FString MinMaxMetadata;
		GetMinMaxMetadataString(MaterialParameter, MinMaxMetadata);

		FString ParameterString = "\tUPROPERTY(EditAnywhere, BlueprintReadWrite, Category=\"" + MaterialParameter.
			Group.ToString() + "\"," + MinMaxMetadata + " DisplayName=\"" + MaterialParameter.Name.ToString() +
			"\")\n\t" +
			MaterialParameter.GetTypeNameAsString() + " " + GetSanitizedParameterName(MaterialParameter.Name) +
			" = " + MaterialParameter.GetValueAsString() + ";\n\n";
		OutMappedLines[MaterialParameter.Group].Append(ParameterString);
	}

	OutMappedLines[Group].Append("\tFOceanology" + SanitizedGroupName + "()\n\t{\n");
	for (FOceanMaterialParameter MaterialParameter : LocalMaterialParameters)
	{
		if (MaterialParameter.Type != Texture)
		{
			continue;
		}

		FString TextureReferencePath =
			"LoadObject<UTexture>(nullptr, TEXT(\"" + MaterialParameter.TextureValue->GetPathName() +
			"\"), nullptr, LOAD_None, nullptr)";
		OutMappedLines[Group].Append(
			"\t\t" + GetSanitizedParameterName(MaterialParameter.Name) + " = " + TextureReferencePath + ";\n");
	}

	// for (FOceanMaterialParameter MaterialParameter : LocalMaterialParameters)
	// {
	// 	if (MaterialParameter.Type != RVT_Texture)
	// 	{
	// 		continue;
	// 	}
	//
	// 	FString TextureReferencePath =
	// 		"LoadObject<URuntimeVirtualTexture>(nullptr, TEXT(\"" + MaterialParameter.RVT_TextureValue->
	// 		GetPathName() +
	// 		"\"), nullptr, LOAD_None, nullptr)";
	// 	MappedLines[Group].Append(
	// 		"\t\t" + GetSanitizedParameterName(MaterialParameter.Name) + " = " + TextureReferencePath + ";\n");
	// }
	OutMappedLines[Group].Append("\t}\n");

	OutMappedLines[Group].Append("};\n\n");
}

void AOceanologyMaterialToStructConverter::MapLinesForHelper(
	const TTuple<FName, TArray<FOceanMaterialParameter>>& MappedParameter,
	const FName Group,
	const FString& SanitizedGroupName,
	TMap<FName, FString>& OutMappedLines
)
{
	TArray<FOceanMaterialParameter> LocalMaterialParameters = MappedParameter.Value;

	MapLinesForHeaderComment(Group, OutMappedLines);

	OutMappedLines[Group].Append(
		"UCLASS()\nclass UOceanology" + SanitizedGroupName + "Helper : public UObject" +
		"\n{\n\tGENERATED_BODY()\n\npublic:\n\tUFUNCTION(BlueprintCallable, Category=\"" + Group.
		ToString() + "\")\n\tstatic void SetMaterialParameters(UMaterialInstanceDynamic* MID, const FOceanology" +
		SanitizedGroupName + "& " + SanitizedGroupName + ")\n\t{\n"
	);

	OutMappedLines[Group].Append("\t\tif (MID == nullptr)\n\t\t{\n\t\t\treturn;\n\t\t}\n\n");

	for (FOceanMaterialParameter MaterialParameter : LocalMaterialParameters)
	{
		switch (MaterialParameter.Type)
		{
		case Vector:
			OutMappedLines[MaterialParameter.Group].Append(
				"\t\tMID->SetVectorParameterValue(\"" + MaterialParameter.Name.ToString() + "\", " +
				SanitizedGroupName + "." + GetSanitizedParameterName(MaterialParameter.Name) + ");\n"
			);
			break;
		case Scalar:
			OutMappedLines[MaterialParameter.Group].Append(
				"\t\tMID->SetScalarParameterValue(\"" + MaterialParameter.Name.ToString() + "\", " +
				SanitizedGroupName + "." + GetSanitizedParameterName(MaterialParameter.Name) + ");\n"
			);
			break;
		case StaticSwitch:
			break;
		case Texture:
			OutMappedLines[MaterialParameter.Group].Append(
				"\t\tMID->SetTextureParameterValue(\"" + MaterialParameter.Name.ToString() + "\", " +
				SanitizedGroupName + "." + GetSanitizedParameterName(MaterialParameter.Name) + ");\n"
			);
			break;
		// case RVT_Texture:
		// 	MappedLines[MaterialParameter.Group].Append(
		// 		"\t\tMID->SetRuntimeVirtualTextureParameterValue(\"" + MaterialParameter.Name.ToString() + "\", " +
		// 		SanitizedGroupName + "." + GetSanitizedParameterName(MaterialParameter.Name) + ");\n"
		// 	);
		// 	break;
		default: ;
		}
	}

	OutMappedLines[Group].Append("\t}\n\n");


	OutMappedLines[Group].Append("\tUFUNCTION(BlueprintPure, Category=\"" + SanitizedGroupName + "\")\n");
	OutMappedLines[Group].Append("\tstatic void Lerp" + SanitizedGroupName + "(\n");
	OutMappedLines[Group].Append("\t\tconst FOceanology" + SanitizedGroupName + "& A, \n");
	OutMappedLines[Group].Append("\t\tconst FOceanology" + SanitizedGroupName + "& B, \n");
	OutMappedLines[Group].Append("\t\tconst double Alpha, \n");
	OutMappedLines[Group].Append("\t\tFOceanology" + SanitizedGroupName + "& OutResult\n");
	OutMappedLines[Group].Append("\t)\n");
	OutMappedLines[Group].Append("\t{\n");

	OutMappedLines[Group].Append("\t\tFOceanology" + SanitizedGroupName + " LocalResult;\n");

	for (FOceanMaterialParameter MaterialParameter : LocalMaterialParameters)
	{
		switch (MaterialParameter.Type)
		{
		case Scalar:
			OutMappedLines[MaterialParameter.Group].Append(
				"\t\tLocalResult." + GetSanitizedParameterName(MaterialParameter.Name) +
				" = UKismetMathLibrary::Lerp(A." + GetSanitizedParameterName(MaterialParameter.Name) +
				", B." + GetSanitizedParameterName(MaterialParameter.Name) +
				", Alpha);\n"
			);
			break;
		default: ;
		}
	}

	OutMappedLines[Group].Append("\t\tOutResult = LocalResult;\n");
	OutMappedLines[Group].Append("\t}\n");

	OutMappedLines[Group].Append("};\n");
}

void AOceanologyMaterialToStructConverter::MapLinesForHeaderComment(
	const FName Group,
	TMap<FName, FString>& OutMappedLines
)
{
	OutMappedLines[Group].Append(
		"/**\n * This is an auto-generated class from Material Parameters via AOceanologyMaterialToStructConverter. DO NOT EDIT BY HAND EVER! Your changes will be lost. Edit the material itself instead!\n * Generated at: "
		+ FText::AsDate(FDateTime::Now(), EDateTimeStyle::Long).ToString() + "\n **/\n"
	);
}

void AOceanologyMaterialToStructConverter::WriteOutputFiles(
	const FName FileName,
	TMap<FName, FString> InMappedLines
) const
{
	for (const TTuple<FName, FString> ContentMap : InMappedLines)
	{
		FString SanitizedGroupName = GetSanitizedGroupName(ContentMap.Key);
		FString FilePath = StructsSourceCodeDirectory + "Oceanology" + SanitizedGroupName + ".h";
		FString FileContent = ContentMap.Value;

		if (FileName != "ANY" && FileName != SanitizedGroupName)
		{
			continue;
		}

		if (FPaths::ValidatePath(FilePath) && FPaths::FileExists(FilePath))
		{
			if (IFileManager& FileManager = IFileManager::Get(); FileManager.Delete(*FilePath))
			{
				UKismetSystemLibrary::PrintString(
					this,
					"Cleaned file: " + FilePath,
					true,
					true,
					FLinearColor::Green,
					1.0
				);
			}
		}

		if (FFileHelper::SaveStringToFile(
			FileContent,
			*FilePath,
			FFileHelper::EEncodingOptions::AutoDetect,
			&IFileManager::Get(),
			FILEWRITE_Append
		))
		{
			UKismetSystemLibrary::PrintString(
				this,
				"Generated file: " + FilePath,
				true,
				true,
				FLinearColor::Green,
				1.0
			);
		}
		else
		{
			UKismetSystemLibrary::PrintString(
				this,
				"Failed to generated file: " + FilePath,
				true,
				true,
				FLinearColor::Red,
				1.0
			);
		}
	}
}

void AOceanologyMaterialToStructConverter::GetMinMaxMetadataString(
	const FOceanMaterialParameter& MaterialParameter,
	FString& MinMaxMetadata
)
{
	MinMaxMetadata = "";

	if (MaterialParameter.ScalarMin != 0 || MaterialParameter.ScalarMax != 0)
	{
		MinMaxMetadata = " meta = (";

		if (MaterialParameter.ScalarMin != 0)
		{
			MinMaxMetadata += "ClampMin = \"" + FString::SanitizeFloat(MaterialParameter.ScalarMin) + "\"";

			if (MaterialParameter.ScalarMax != 0)
			{
				MinMaxMetadata += ",";
			}
		}

		if (MaterialParameter.ScalarMax != 0)
		{
			MinMaxMetadata += "ClampMax = \"" + FString::SanitizeFloat(MaterialParameter.ScalarMax) + "\"";
		}

		MinMaxMetadata += "),";
	}
}

FString AOceanologyMaterialToStructConverter::GetSanitizedGroupName(const FName Group)
{
	return Group.ToString()
	            .Replace(TEXT(" "), TEXT(""))
	            .Replace(TEXT("("), TEXT(""))
	            .Replace(TEXT(")"), TEXT(""));
}

FString AOceanologyMaterialToStructConverter::GetSanitizedParameterName(const FName Name)
{
	return Name.ToString()
	           .Replace(TEXT(" "), TEXT(""))
	           .Replace(TEXT("("), TEXT(""))
	           .Replace(TEXT(")"), TEXT(""));
}
