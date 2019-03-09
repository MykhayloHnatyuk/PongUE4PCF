// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PNGPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class PONGPCF_API APNGPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	
	bool IsGoodForPlayerOne() const { return IsReservedForPlayerOne; }

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|Start Settings")
	bool IsReservedForPlayerOne;

};
