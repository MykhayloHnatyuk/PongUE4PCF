// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PNGGameModeLobby.generated.h"

class UPNGLobbyWidget;

UCLASS()
class PONGPCF_API APNGGameModeLobby : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	void HostGame();
	void JoinGame(const FString& Ip);

	UPROPERTY(EditDefaultsOnly, Category = "Pong|LobbyMenu")
	TSubclassOf<UPNGLobbyWidget> lobbyMenuType;
};
