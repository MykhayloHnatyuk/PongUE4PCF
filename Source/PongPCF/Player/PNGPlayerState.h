// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PNGPlayerState.generated.h"

UCLASS()
class PONGPCF_API APNGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UFUNCTION()
	virtual void OnRep_IsReady();

	UFUNCTION()
	virtual void OnRep_IsPlayerOne();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	// If we can start the play. 
	UPROPERTY(replicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category = PlayerState)
	bool bIsReady;

	UPROPERTY(replicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category = PlayerState)
	bool bIsPlayerOne;
};
