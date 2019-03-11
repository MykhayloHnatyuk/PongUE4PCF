// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGoalZone.h"

APNGGoalZone::APNGGoalZone()
{
	PrimaryActorTick.bCanEverTick = true;

}

bool APNGGoalZone::IsPlayerOneGoalZone() const
{
	//TODO: Find a better way to determine it. 
	return GetActorLocation().Y < 0;
}

void APNGGoalZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void APNGGoalZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

