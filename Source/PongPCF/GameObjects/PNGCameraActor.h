// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Settings/PNGGameStateMain.h"
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

	// Default camera tint color angle on a Color Wheel.
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float DefaultColorAngle;
	// Color angle step that is applied when a ball hits something.
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float HitColorUpdateStep;

	// Default speed if interpolation through the Color Wheel.
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float DefaultColorUpdateSpeed;

	// Speed if interpolation through the Color Wheel when starting/restarting a play.
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float StartingPlayColorUpdateSpeed;

	// Speed if interpolation through the Color Wheel when play was finished.
	UPROPERTY(EditAnywhere, Category = "Pong|Tint Change")
	float PlayFinishedColorUpdateSpeed;

	// Color curve used to interpolate through the custom Color Wheel.
	UPROPERTY(EditDefaultsOnly, Category = "Pong|Tint Change")
	UCurveLinearColor* ColorCurve;

private:

	void BindBallHitActorEvent();

	UFUNCTION()
	void OnGameStateChangedHandler(PNGGameState NewState);

	UFUNCTION()
	void OnBallHitActorHandler(AActor* HitActor, FVector HitLocation);

	float mColorUpdateSpeed;
	float mColorAngle;
};
