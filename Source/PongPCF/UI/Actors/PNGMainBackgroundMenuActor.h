// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Settings/PNGGameStateMain.h"
#include "GameFramework/Actor.h"
#include "PNGMainBackgroundMenuActor.generated.h"

class UWidgetComponent;

UCLASS()
class PONGPCF_API APNGMainBackgroundMenuActor : public AActor
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitializeBP(UWidgetComponent* MainWidgetComponent);

private:

	UFUNCTION()
	void OnGameStateChangedHandler(PNGGameState NewState);
	UFUNCTION()
	void OnScoreChangedHandler(int Player1Score, int Player2Score);

	class UPNGMainBackgroundWidget* mMainMenuWidget;
};
