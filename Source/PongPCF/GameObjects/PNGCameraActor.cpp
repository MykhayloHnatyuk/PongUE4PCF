// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGCameraActor.h"
#include "Engine.h"
#include "Settings/PNGGameModeMain.h"
#include "GameObjects/PNGBall.h"

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

	APNGGameStateMain* gs = Cast<APNGGameStateMain>(GetWorld()->GetGameState());
	gs->OnGameStateChanged().RemoveAll(this);
	gs->OnGameStateChanged().AddUObject(this, &APNGCameraActor::OnGameStateChangedHandler);

	BindBallHitActorEvent();
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

void APNGCameraActor::BindBallHitActorEvent()
{
	// Since the ball is created from GameMode on server,
	// and we don't have GameMode on a client,
	// and we don't have much actors on scene - lets find the ball by actor iteration.

	UWorld* World = GetWorld();

	for (TActorIterator<APNGBall> It(World); It; ++It)
	{
		APNGBall* Ball = *It;
		Ball->OnBallHitActor().RemoveAll(this);
		Ball->OnBallHitActor().AddUObject(this, &APNGCameraActor::OnBallHitActorHandler);

		break;
	}
}

void APNGCameraActor::OnGameStateChangedHandler(PNGGameState NewState)
{
	switch (NewState)
	{
	case PNGGameState::gsWaitingForPlayers:
		break;
	case PNGGameState::gsStartingPlay:
		mColorUpdateSpeed = StartingPlayColorUpdateSpeed;
		break;
	case PNGGameState::gsPlaying:
		mColorUpdateSpeed = DefaultColorUpdateSpeed;
		break;
	default:
		break;
	}
}

void APNGCameraActor::OnBallHitActorHandler(AActor* HitActor, FVector HitLocation)
{
	mColorAngle += HitColorUpdateStep;
}

