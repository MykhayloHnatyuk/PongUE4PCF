// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGameModeLobby.h"
#include "Engine.h"
#include "UI/PNGLobbyWidget.h"

#define HOST_COMMAND "open Main?listen"

void APNGGameModeLobby::BeginPlay()
{
	Super::BeginPlay();

	if(auto menu = CreateWidget<UPNGLobbyWidget>(GetWorld()->GetGameInstance(), lobbyMenuType))
	{
		menu->AddToViewport();
	}
}

void APNGGameModeLobby::HostGame()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->ConsoleCommand(TEXT(HOST_COMMAND), true);
}

#define JOIN_COMMAND "open %s"
void APNGGameModeLobby::JoinGame(const FString& Ip)
{
	//TODO: Validate Ip.
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->ConsoleCommand(FString::Printf(TEXT(JOIN_COMMAND), *Ip), true);
}
