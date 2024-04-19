// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanologyInfiniteComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FInfiniteLocationChanged,
	FVector, CameraLocation,
	FVector, LastLocation,
	FVector, NewParentLocation
);

class UWorld;

UCLASS(HideCategories =(AssetUserData, Activation, Tags, Cooking), ClassGroup=(Custom),
	meta=(BlueprintSpawnableComponent))
class OCEANOLOGY_PLUGIN_API UOceanologyInfiniteComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanologyInfiniteComponent();

	UPROPERTY(Category = "Infinite", BlueprintReadWrite, EditAnywhere,
		meta = (ToolTip = "Enable/disable the Infinite follow system."))
	bool EnableInfinity = true;

	UPROPERTY(Category = "Infinite", BlueprintReadWrite, EditAnywhere,
		meta = (ToolTip = "Enable/disable the Infinite follow system in editor. Affects ONLY the editor!"))
	bool EnableInfinityInEditor = true;

	UPROPERTY(Category = "Infinite", BlueprintReadWrite, EditAnywhere,
		meta = (ToolTip = "Enable/disable the Infinite follow system in game. Affects ONLY the game after BeginPlay!"))
	bool EnableInfinityInGame = true;

	UPROPERTY(Category = "Infinite", BlueprintReadWrite, EditAnywhere,
		meta = (ToolTip =
			"Value 0 means real-time update checking (not real-time relocation!). Increasing the value provides better performance via delayed updates. This affects how often the Event Tick checks for follow requests."
		))
	float FollowUpdateInterval = 0;

	UPROPERTY(Category = "Infinite", BlueprintReadWrite, EditAnywhere,
		meta = (ToolTip =
			"This controls the distance based on GridJump when the follow request happens. 0 = real-time relocation IF FollowUpdateInterval is = 0 as well."
		))
	float TimeJump = 20000.0f;

	UPROPERTY(Category = "Infinite|Debug", BlueprintReadWrite, EditAnywhere,
		meta = (ToolTip = "Enable/disable debug messages of the infinite component."))
	bool EnableDebug = false;

	UPROPERTY(BlueprintAssignable, Category = "Infinite")
	FInfiniteLocationChanged OnInfiniteLocationChanged;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		const float DeltaTime,
		const ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	UFUNCTION(BlueprintCallable, Category="Infinite",
		meta = (ToolTip = "Can be used to call a force follow on the actor when follow in editor is disabled."))
	void ForceFollow();

	UFUNCTION()
	void Follow(const bool ForceFollow);

	UFUNCTION(BlueprintCallable, Category="Infinite")
	void Init(AActor* Actor, UWorld* InOrigin);

	UFUNCTION(BlueprintCallable, Category="Infinite")
	void SetOrigin(UWorld* InOrigin);

	UFUNCTION(BlueprintCallable, Category="Infinite")
	void InitDefaultOrigin();

	UFUNCTION(BlueprintCallable, Category="Infinite")
	void SetParentActor(AActor* Actor);

private:
	UPROPERTY()
	UWorld* Origin = nullptr;

	UPROPERTY()
	AActor* ParentActor = nullptr;

	UPROPERTY()
	FVector LastLocation = FVector::ZeroVector;

	void SetLocation(const FVector& CameraLocation);
};
