// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PNGCameraActor.generated.h"

UCLASS()
class PONGPCF_API APNGCameraActor : public ACameraActor
{
	GENERATED_BODY()

protected:

	APNGCameraActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void UpdateColor(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float DefaultColorUpdateSpeed;
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float StartingPlayColorUpdateSpeed;
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float HitColorUpdateStep;

	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float DefaultColorAngle;

	UPROPERTY(EditDefaultsOnly, Category = "Pong|Tint Change")
	UCurveLinearColor* ColorCurve;

private:

	float mColorUpdateSpeed;
	float mColorAngle;
};
