// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Returns fixed server time from PNGGameState.
#define GetFixedServerTime() GetFixedServerWorldTimeSeconds(GetWorld())

float GetFixedServerWorldTimeSeconds(UWorld* Wold);