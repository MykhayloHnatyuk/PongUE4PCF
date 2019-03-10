// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGGoalZone.h"

// Sets default values
APNGGoalZone::APNGGoalZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool APNGGoalZone::IsPlayerOneGoalZone() const
{
	// TODO: Find a better way to do it.
	return GetActorLocation().Y < 0;
}

// Called when the game starts or when spawned
void APNGGoalZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APNGGoalZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

