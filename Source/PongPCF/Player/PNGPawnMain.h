// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PNGPawnMain.generated.h"

UCLASS()
class PONGPCF_API APNGPawnMain : public APawn
{
	GENERATED_BODY()

public:

	APNGPawnMain();

	float GetMovementSpeed() const { return MovementSpeed; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|PlayerParams")
	float MovementSpeed;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Limits OutTargetedLocation if it will hit something on attempt to change location.
	void LimitMoveToLocation(FVector& OutTargetedLocation) const;

};
