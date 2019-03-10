// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PNGGoalZone.generated.h"

UCLASS()
class PONGPCF_API APNGGoalZone : public AActor
{
	GENERATED_BODY()
	
public:	

	APNGGoalZone();

	bool IsPlayerOneGoalZone() const;

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
