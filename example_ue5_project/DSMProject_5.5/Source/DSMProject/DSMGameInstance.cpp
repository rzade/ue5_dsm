/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */


#include "DSMGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "OnlineSessionSettings.h"
#include "Misc/CoreDelegates.h"
#include "TimerManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Blueprint/UserWidget.h"
#include "StartWidget.h"

UDSMGameInstance::UDSMGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> StartWidgetBPClass(TEXT("/Game/Widgets/WBP_Start"));
	if (!ensure(StartWidgetBPClass.Class != nullptr)) return;
	StartWidgetClass = StartWidgetBPClass.Class;
}

void UDSMGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDSMGameInstance::OnCreateSesionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UDSMGameInstance::OnDestroySesionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UDSMGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UDSMGameInstance::OnJoinSessionComplete);
			SessionInterface->OnUpdateSessionCompleteDelegates.AddUObject(this, &UDSMGameInstance::OnUpdateSessionCompleted);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
	}

	if (GEngine != nullptr) {
		GEngine->OnNetworkFailure().AddUObject(this, &UDSMGameInstance::OnNetworkFailure);
	}
}

void UDSMGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) {
	
}

void UDSMGameInstance::Shutdown()
{
	
}

void UDSMGameInstance::OnDestroySesionComplete(FName SessionName, bool Success) {

}

void UDSMGameInstance::OnUpdateSessionCompleted(FName SessionName, bool Success) {

}

void UDSMGameInstance::OnCreateSesionComplete(FName SessionName, bool Success) {

}

void UDSMGameInstance::OnFindSessionsComplete(bool Success) {
	
}

void UDSMGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	
}

void UDSMGameInstance::Join(FString Address) {
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController && !Address.IsEmpty()) {
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UDSMGameInstance::ServerList() {
	FString RequestBody;
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();

	FString ProjectName = FApp::GetProjectName();

	RequestObj->SetStringField("authorization", AuthKey);
	RequestObj->SetStringField("projectname", FString::Printf(TEXT("%sServer"), *ProjectName));

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj, Writer);

	Request->SetURL(FString::Printf(TEXT("http://%s/api/server_list"), *ServerAddress));
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully) mutable {
		if (connectedSuccessfully) {
			if (StartWidget) {
				StartWidget->ServerListString = pResponse->GetContentAsString();
				StartWidget->ServerListIsReady();
			}
		}
	});
	Request->ProcessRequest();
}

void UDSMGameInstance::UpdateServer(int32 onlineplayer) {
	
}

void UDSMGameInstance::StartMenu() {
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	if (!ensure(StartWidgetClass != nullptr)) return;

	StartWidget = CreateWidget<UStartWidget>(this, StartWidgetClass);
	if (!ensure(StartWidget != nullptr)) return;

	StartWidget->AddToViewport();

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(StartWidget->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}