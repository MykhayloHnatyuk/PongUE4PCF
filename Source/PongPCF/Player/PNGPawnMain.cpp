// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGPawnMain.h"

APNGPawnMain::APNGPawnMain()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APNGPawnMain::BeginPlay()
{
	Super::BeginPlay();
	
}

void APNGPawnMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APNGPawnMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

