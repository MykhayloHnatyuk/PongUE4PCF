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

	void SetWasUsed(bool Value) { mWasUsed = Value; };
	bool GetWasUsed() const { return mWasUsed; }

private:

	bool mWasUsed;
};
