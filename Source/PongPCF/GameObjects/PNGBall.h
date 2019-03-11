// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PNGBall.generated.h"

class APNGGoalZone;

USTRUCT()
struct FPush
{
	GENERATED_USTRUCT_BODY()

	FPush()
	{
		StartLocation = FVector::ZeroVector;
		Direction = FVector::ZeroVector;
		Time = 0.0f;
		Speed = 0.0f;
	}

	FPush(const FVector& StartLocation, float Time) : FPush()
	{
		this->StartLocation = StartLocation;
		this->Time = Time;
	}

	UPROPERTY()
	FVector StartLocation;
	UPROPERTY()
	FVector Direction;
	UPROPERTY()
	float Time;
	UPROPERTY()
	float Speed;
};

UCLASS()
class PONGPCF_API APNGBall : public AActor
{
	GENERATED_BODY()
	
public:	

	APNGBall();

	// Default speed used at the start of each play.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|Ball Stats")
	float InitialSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|Ball Stats")
	float MaxSpeed;
	// Defines how speed will be increased (when pawn hits a ball) in percentage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|Ball Stats")
	float SpeedIncreasePrcntg;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	void StopBallAtLocation(FVector Location);
	void PushBallInRandomDirection();

	DECLARE_EVENT_OneParam(APNGBall, FPNGBallHitGoalEvent, APNGGoalZone*)
	FPNGBallHitGoalEvent& OnBallHitGoal() { return BallHitGoalEvent; }

	DECLARE_EVENT_TwoParams(APNGBall, FPNGBallHitActorEvent, AActor*, FVector)
	FPNGBallHitActorEvent& OnBallHitActor() { return BallHitActorEvent; }

private:

	void UpdateLocation();

	FVector GetLocationByTime(float Time) const;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCUpdatePushData(FPush Push);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mSphereComp;

	FPNGBallHitGoalEvent BallHitGoalEvent;
	FPNGBallHitActorEvent BallHitActorEvent;

	UPROPERTY()
	FPush mLastPush;
};
