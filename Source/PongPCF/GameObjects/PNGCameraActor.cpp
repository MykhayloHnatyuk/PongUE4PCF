// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGCameraActor.h"
#include "Engine.h"

void APNGCameraActor::BeginPlay()
{
	Super::BeginPlay();

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