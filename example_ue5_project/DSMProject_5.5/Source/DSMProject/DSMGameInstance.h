/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DSMGameInstance.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class DSMPROJECT_API UDSMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UDSMGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(config, BlueprintReadOnly)
	FString ServerAddress;

	UPROPERTY(config)
	FString AuthKey;
	
	virtual void Init();
	virtual void Shutdown();

	UFUNCTION(BlueprintCallable)
	void StartMenu();

	UFUNCTION(BlueprintCallable)
	void ServerList();

	UFUNCTION(BlueprintCallable)
	void UpdateServer(int32 onlineplayer);

	UFUNCTION(BlueprintCallable)
	void Join(FString Address);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSesionComplete(FName SessionName, bool Success);
	void OnDestroySesionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnUpdateSessionCompleted(FName SessionName, bool Success);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	UPROPERTY(BlueprintReadOnly)
	class UStartWidget* StartWidget;

private:
	TSubclassOf<class UUserWidget> StartWidgetClass;
};
