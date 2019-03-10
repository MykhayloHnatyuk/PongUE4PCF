// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGlobalDefines.h"
#include "Settings/PNGGameStateMain.h"

float GetFixedServerWorldTimeSeconds(UWorld* World)
{
	float result = 0.0f;

	if(World->IsValidLowLevel())
	{
		if(auto gs = Cast<APNGGameStateMain>(World->GetGameState()))
		{
			result = gs->GetFixedServerWorldTimeSeconds();
		}
	}	

	return result;
}