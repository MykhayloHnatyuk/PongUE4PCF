// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PNGBall.generated.h"

UCLASS()
class PONGPCF_API APNGBall : public AActor
{
	struct FPush
	{
		FPush()
		{
			StartLocation = FVector::ZeroVector;
			Direction = FVector::ZeroVector;
			Time = 0.0f;
		}

		FVector StartLocation;
		FVector Direction;
		float Time;
	};

	GENERATED_BODY()
	
public:	

	APNGBall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|Ball Stats")
	float Speed;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	void StopBallAtLocation(FVector Location);
	void PushBallInRandomDirection();

private:

	void UpdateLocation();

	FVector GetLocationByTime(float Time) const;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCUpdatePushData(FVector Start, FVector Direction, float Time);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mSphereComp;

	FPush mLastPush;
};
