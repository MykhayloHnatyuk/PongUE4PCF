// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGMainBackgroundWidget.h"
#include "Components/TextBlock.h"

void UPNGMainBackgroundWidget::InitializeBP(UTextBlock* Status, UTextBlock* Player1Score, UTextBlock* Player2Score)
{
	mStatusText = Status;
	mPlayer1ScoreText = Player1Score;
	mPlayer2ScoreText = Player2Score;

	UE_LOG(LogType, Log, TEXT("PNGMainBackgroundWidget::InitializeBP"));
}

void UPNGMainBackgroundWidget::UpdateStatus(const FString& Value)
{
	UE_LOG(LogType, Log, TEXT("PNGMainBackgroundWidget::UpdateStatus"));

	if (mStatusText)
	{

	
	ensure(mStatusText);

	mStatusText->SetText(FText::FromString(Value));
}

}

void UPNGMainBackgroundWidget::UpdateScore(int Player1Score, int Player2Score)
{
	ensure(mPlayer1ScoreText);
	ensure(mPlayer2ScoreText);

	mPlayer1ScoreText->SetText(FText::AsNumber(Player1Score));
	mPlayer2ScoreText->SetText(FText::AsNumber(Player2Score));
}
