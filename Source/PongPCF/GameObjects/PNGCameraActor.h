// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PNGCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class PONGPCF_API APNGCameraActor : public ACameraActor
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	
};
