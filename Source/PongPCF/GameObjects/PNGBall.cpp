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
	if (GetWorld()->IsServer())
	{
		FPush push = FPush(Location, GetFixedServerTime());
		MulticastRPCUpdatePushData(push);
	}	
}

#define RANDOM_DIRECTION_POWER FVector(1.0f, 1.0f, 0.0f)
void APNGBall::PushBallInRandomDirection()
{
	if (!GetWorld()->IsServer())
	{
		UE_LOG(LogType, Error, TEXT("APNGBall::PushBallInRandomDirection called not on server!"));
		return;
	}

	FPush push = FPush(GetActorLocation(), GetFixedServerTime());

	// Create random direction for initial push,
	// and let's make sure it won't be too vertical.
	FVector randDirection = (FMath::VRand() * RANDOM_DIRECTION_POWER).GetSafeNormal();
	if(abs(randDirection.X) > abs(randDirection.Y))
	{
		randDirection = FVector(randDirection.Y, randDirection.X, 0);
	}

	push.Direction = randDirection;
	// Update speed to default at every initial push.
	push.Speed = InitialSpeed;

	MulticastRPCUpdatePushData(push);
}

void APNGBall::UpdateLocation()
{
	float serverTime = GetFixedServerTime();
	const FVector newLocation = GetLocationByTime(serverTime);
	SetActorLocation(newLocation, true);
}

FVector APNGBall::GetLocationByTime(float Time) const
{
	FVector result = mLastPush.StartLocation;
	float timePassed = Time - mLastPush.Time;
	result += mLastPush.Direction * mLastPush.Speed * timePassed;

	return result;
}

#define PAWN_HIT_POINT_INFLUENCE_POWER 2.0f
void APNGBall::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Broadcast event for visual effects on all clients.
	OnBallHitActor().Broadcast(OtherActor, SweepResult.ImpactPoint);

	if (!GetWorld()->IsServer())
	{
		return;
	}

	if (OtherActor != nullptr)
	{
		// Initialize new push.
		FPush newPush = FPush(GetActorLocation(), GetFixedServerTime());
		// We might change Speed later.
		newPush.Speed = mLastPush.Speed;

		// Now let's, calculate new direction.
		FVector newDirection = FVector::ZeroVector;

		auto ReflectVector = [](const FVector& InVector, const FVector& Normal) -> FVector
		{
			return InVector - FVector::DotProduct(Normal, InVector) * Normal * 2;
		};

		if(auto pawn = Cast<APNGPawnMain>(OtherActor))
		{
			const FVector hitPlaneNormal = pawn->GetActorForwardVector();
			const FVector reflectedDirection = ReflectVector(mLastPush.Direction, hitPlaneNormal);
			
			// Edit final direction based on a hit location just to make things more unpredictable for an other player.
			newDirection = reflectedDirection + (GetActorLocation() - pawn->GetActorLocation()).GetSafeNormal() * PAWN_HIT_POINT_INFLUENCE_POWER;
			
			// Let's increase speed after each Pawn hit.
			newPush.Speed = FMath::Clamp(mLastPush.Speed + mLastPush.Speed * (SpeedIncreasePrcntg / 100.0f), InitialSpeed, MaxSpeed);
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

		// Just to make sure that we will never move out of 2D space.
		newDirection *= FVector(1, 1, 0);
		newPush.Direction = newDirection.GetSafeNormal();

		MulticastRPCUpdatePushData(newPush);
	}
}

void APNGBall::MulticastRPCUpdatePushData_Implementation(FPush Push)
{
	mLastPush.StartLocation = Push.StartLocation;
	mLastPush.Direction = Push.Direction.GetSafeNormal();
	mLastPush.Time = Push.Time;
	mLastPush.Speed = Push.Speed;
}