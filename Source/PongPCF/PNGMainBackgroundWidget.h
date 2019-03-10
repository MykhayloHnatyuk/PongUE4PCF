// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PNGMainBackgroundWidget.generated.h"

class UTextBlock;

UCLASS()
class PONGPCF_API UPNGMainBackgroundWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	void InitializeBP(UTextBlock* Status, UTextBlock* Player1Score, UTextBlock* Player2Score);

public:

	void UpdateStatus(const FString& Value);
	void UpdateScore(int Player1Score, int Player2Score);

private:
	
	UTextBlock* mStatusText;
	UTextBlock* mPlayer1ScoreText;
	UTextBlock* mPlayer2ScoreText;
};
