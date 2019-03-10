// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PNGLobbyWidget.generated.h"

class UButton;
class UEditableTextBox;

UCLASS()
class PONGPCF_API UPNGLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void InitializeBP(UButton* HostButton, UButton* JoinButton, UEditableTextBox* IpTextBox);

private:

	UFUNCTION()
	void OnHostButtonClickHandler();
	UFUNCTION()
	void OnJoinButtonClickHandler();

	UButton* mHostButton;
	UButton* mJoinButton;
	UEditableTextBox* mIpTextBox;
};
