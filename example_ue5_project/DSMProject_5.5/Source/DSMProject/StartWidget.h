/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class DSMPROJECT_API UStartWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ServerList;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* JoinServer;

	UPROPERTY(BlueprintReadOnly)
	FString ServerListString;

	UPROPERTY(BlueprintReadWrite)
	int32 SelectedServerIndex = -1;

	UFUNCTION()
	void ServerListFunc();

	UFUNCTION()
	void JoinServerFunc();

	UFUNCTION(BlueprintImplementableEvent)
	void ServerListIsReady();

protected:
	virtual bool Initialize();
};
