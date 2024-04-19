// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Utils/OceanologyMessageUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif

void UOceanologyMessageUtils::CreateMessage( 
	const UObject* WorldContextObject,
	const FString& Message,
	const bool bSuccess
)
{
	// This should act as a simple debug message IF begin play already happened.
	if (WorldContextObject->GetWorld()
		&& WorldContextObject->GetWorld()->HasBegunPlay())
	{
		UKismetSystemLibrary::PrintString(
			WorldContextObject,
			Message,
			true,
			true,
			bSuccess ? FLinearColor::Green : FLinearColor::Red,
			1.0
		);
		return;
	}

#if WITH_EDITOR
	FNotificationInfo Info(FText::FromString(Message));
	Info.ExpireDuration = 3.0f;
	const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
	Notification->SetCompletionState(bSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);

	if (bSuccess)
	{
		UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s"), *Message);
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Warning, TEXT("%s"), *Message);
	}
#endif
}
