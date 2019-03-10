// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGBall.h"
#include "Components/StaticMeshComponent.h"
#include "Settings/PNGGameStateMain.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "PNGGlobalDefines.h"
#include "PNGGoalZone.h"
#include "Player/PNGPawnMain.h"

#define SPHERE_COMP_NAME "Sphere"
APNGBall::APNGBall()
{
	PrimaryActorTick.bCanEverTick = true;

	mSphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT(SPHERE_COMP_NAME));
	mSphereComp->SetupAttachment(RootComponent);
	mSphereComp->OnComponentBeginOverlap.AddDynamic(this, &APNGBall::OnBeginOverlap);
}

void APNGBall::BeginPlay()
{
	Super::BeginPlay();
}

void APNGBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLocation();
}

void APNGBall::StopBallAtLocation(FVector Location)
{
	if (!GetWorld()->IsServer())
	{
		UE_LOG(LogType, Log, TEXT("APNGBall::StopBallAtLocation not server."));
		return;
	}

	float serverTime = GetFixedServerTime();
	MulticastRPCUpdatePushData(Location, FVector::ZeroVector, serverTime);
}

#define RANDOM_DIRECTION_POWER FVector(1.0f, 5.0f, 0.0f)
void APNGBall::PushBallInRandomDirection()
{
	if (!GetWorld()->IsServer())
	{
		UE_LOG(LogType, Log, TEXT("APNGBall::PushBallInRandomDirection not server."));
		return;
	}

	const FVector& currLocation = GetActorLocation();

	FVector randDirection = FMath::VRand() * RANDOM_DIRECTION_POWER;
	randDirection = randDirection.GetSafeNormal();

	float serverTime = GetFixedServerTime();

	MulticastRPCUpdatePushData(currLocation, randDirection, serverTime);
}

void APNGBall::UpdateLocation()
{
	if (!GetWorld())
	{
		UE_LOG(LogType, Error, TEXT("APNGBall::UpdateLocation GetWorld() not valid!"));
		return;
	}

	float serverTime = GetFixedServerTime();
	const FVector newLocation = GetLocationByTime(serverTime);

	SetActorLocation(newLocation, true);
}

FVector APNGBall::GetLocationByTime(float Time) const
{
	FVector result = mLastPush.StartLocation;
	float timePassed = Time - mLastPush.Time;
	result += mLastPush.Direction * Speed * timePassed;

	return result;
}

#define PAWN_HIT_POINT_INFLUENCE_POWER 2.0f
void APNGBall::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OnBallHitActor().Broadcast(OtherActor, SweepResult.ImpactPoint);

	/*if (!GetWorld()->IsServer())
	{
		UE_LOG(LogType, Log, TEXT("APNGBall::OnBeginOverlap not server."));
		return;
	}*/

	if (OtherActor != nullptr)
	{
		auto ReflectVector = [](const FVector& InVector, const FVector& Normal) -> FVector
		{
			return InVector - FVector::DotProduct(Normal, InVector) * Normal * 2;
		};

		FVector newDirection = FVector::ZeroVector;

		if(auto pawn = Cast<APNGPawnMain>(OtherActor))
		{
			const FVector hitPlaneNormal = pawn->GetActorForwardVector();
			const FVector reflectedDirection = ReflectVector(mLastPush.Direction, hitPlaneNormal);
			
			// Edit final direction based on a hit location just to make things more unpredictable for an other player.
			newDirection = reflectedDirection + (GetActorLocation() - pawn->GetActorLocation()).GetSafeNormal() * PAWN_HIT_POINT_INFLUENCE_POWER;
		}
		else if (auto goal = Cast<APNGGoalZone>(OtherActor))
		{
			OnBallHitGoal().Broadcast(goal);
		}
		else
		{
			const FVector hitPlaneNormal = (SweepResult.Normal * FVector(1, 1, 0)).GetSafeNormal();
			newDirection = ReflectVector(mLastPush.Direction, hitPlaneNormal);
		}

		float serverTime = GetFixedServerTime();

		// Just to make that we will never move out of 2D space.
		newDirection *= FVector(1, 1, 0);

		MulticastRPCUpdatePushData(GetActorLocation(), newDirection.GetSafeNormal(), serverTime);
	}
}

void APNGBall::MulticastRPCUpdatePushData_Implementation(FVector Start, FVector Direction, float Time)
{
	mLastPush.StartLocation = Start;
	mLastPush.Direction = Direction.GetSafeNormal();
	mLastPush.Time = Time;
}