// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGLobbyWidget.h"
#include "Button.h"
#include "EditableTextBox.h"
#include "Settings/PNGGameModeLobby.h"

void UPNGLobbyWidget::InitializeBP(UButton* HostButton, UButton* JoinButton, UEditableTextBox* IpTextBox)
{
	ensure(HostButton);
	ensure(JoinButton);
	ensure(IpTextBox);

	mHostButton = HostButton;
	mJoinButton = JoinButton;
	mIpTextBox = IpTextBox;

	mHostButton->OnClicked.RemoveAll(this);
	mHostButton->OnClicked.AddDynamic(this, &UPNGLobbyWidget::OnHostButtonClickHandler);
	mJoinButton->OnClicked.RemoveAll(this);
	mJoinButton->OnClicked.AddDynamic(this, &UPNGLobbyWidget::OnJoinButtonClickHandler);
}

void UPNGLobbyWidget::OnHostButtonClickHandler()
{
	auto lobbyMode = GetWorld()->GetAuthGameMode<APNGGameModeLobby>();
	lobbyMode->HostGame();
}

void UPNGLobbyWidget::OnJoinButtonClickHandler()
{
	auto lobbyMode = GetWorld()->GetAuthGameMode<APNGGameModeLobby>();
	lobbyMode->JoinGame(mIpTextBox->Text.ToString());
}
