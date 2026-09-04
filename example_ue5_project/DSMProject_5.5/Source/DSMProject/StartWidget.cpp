/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */


#include "StartWidget.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "DSMGameInstance.h"

bool UStartWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(ServerList != nullptr)) return false;
	ServerList->OnClicked.AddDynamic(this, &UStartWidget::ServerListFunc);

	if (!ensure(JoinServer != nullptr)) return false;
	JoinServer->OnClicked.AddDynamic(this, &UStartWidget::JoinServerFunc);

	return true;
}

void UStartWidget::ServerListFunc() {
	UDSMGameInstance* DSMGI = Cast<UDSMGameInstance>(GetGameInstance());
	if (!ensure(DSMGI != nullptr)) return;
	DSMGI->ServerList();
}

void UStartWidget::JoinServerFunc() {
	UDSMGameInstance* DSMGI = Cast<UDSMGameInstance>(GetGameInstance());
	if (!ensure(DSMGI != nullptr)) return;

	if (!ServerListString.IsEmpty() && SelectedServerIndex > -1) {
		TSharedPtr<FJsonObject> JsonObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ServerListString);
		if (FJsonSerializer::Deserialize(Reader, JsonObj)) {
			auto Servers = JsonObj->GetArrayField("servers");
			auto Serv = Servers[SelectedServerIndex]->AsObject();
			FString JoinLink = FString::Printf(TEXT("%s:%s"), *Serv->GetStringField("ip"), *Serv->GetStringField("port"));
			UE_LOG(LogTemp, Warning, TEXT("JoinLink - %s"), *JoinLink);
			DSMGI->Join(JoinLink);
		}
	}
}


