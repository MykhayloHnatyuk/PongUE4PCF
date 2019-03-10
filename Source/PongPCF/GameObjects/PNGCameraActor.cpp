// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGCameraActor.h"
#include "Engine.h"

APNGCameraActor::APNGCameraActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GetCameraComponent()->PostProcessSettings.bOverride_SceneColorTint = true;
}

void APNGCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
	mColorAngle = DefaultColorAngle;
	mColorUpdateSpeed = DefaultColorUpdateSpeed;

	if (GetWorld())
	{
		if (auto* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			pc->SetViewTargetWithBlend(this);
		}
		else
		{
			UE_LOG(LogType, Error, TEXT("APNGCameraActor::BeginPlay PlayerController not found!"));
		}
	}
}

void APNGCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateColor(DeltaTime);
}

void APNGCameraActor::UpdateColor(float DeltaTime)
{
	// Lets imagine that we have a color wheel where red is at 0 degree, green at 120 and blue at 240
	// https://en.wikipedia.org/wiki/File:Simple_RGB_color_wheel.png
	// We will interpolate through it.

	mColorAngle += mColorUpdateSpeed * DeltaTime;
	mColorAngle = FMath::Fmod(mColorAngle, 360.0f);
	
	const FLinearColor color = ColorCurve->GetClampedLinearColorValue(mColorAngle);
	GetCameraComponent()->PostProcessSettings.SceneColorTint = FLinearColor(color);
}

