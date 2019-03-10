// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PNGPlayerControllerLobby.generated.h"

/**
 * 
 */
UCLASS()
class PONGPCF_API APNGPlayerControllerLobby : public APlayerController
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;
};
