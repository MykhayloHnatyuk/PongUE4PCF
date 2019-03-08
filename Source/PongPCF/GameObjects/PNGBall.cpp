// Fill out your copyright notice in the Description page of Project Settings.

#include "PNGBall.h"
#include "Components/StaticMeshComponent.h"
#include "Settings/PNGGameStateMain.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#pragma optimize ("", on)

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

	MulticastRPCUpdatePushData(Location, FVector::ZeroVector, GetServerTime());
}

#define RANDOM_DIRECTION_POWER FVector(0.5f, 1.0f, 0.0f)
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

	float serverTime = GetServerTime();

	MulticastRPCUpdatePushData(currLocation, randDirection, serverTime);
}

void APNGBall::UpdateLocation()
{
	if (!GetWorld())
	{
		UE_LOG(LogType, Error, TEXT("APNGBall::UpdateLocation GetWorld() not valid!"));
		return;
	}

	float serverTime = GetServerTime();
	const FVector newLocation = GetLocationByTime(serverTime);

	SetActorLocation(newLocation, true);
/*
	FColor color = (GetWorld()->IsServer()) ? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(-1, 0.02f, color, FString::Printf(TEXT("APNGBall::UpdateLocation time: %f newLocation %s"), serverTime, *newLocation.ToString()));*/
}

FVector APNGBall::GetLocationByTime(float Time) const
{
	FVector result = mLastPush.StartLocation;
	float timePassed = Time - mLastPush.Time;
	result += mLastPush.Direction * Speed * timePassed;

	return result;
}

float APNGBall::GetServerTime() const
{
	const AGameStateBase* gs = GetWorld()->GetGameState();
	return gs->GetServerWorldTimeSeconds();
}

void APNGBall::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!GetWorld()->IsServer())
	{
		UE_LOG(LogType, Log, TEXT("APNGBall::OnBeginOverlap not server."));
		return;
	}

	if (OtherActor != nullptr)
	{
		UE_LOG(LogType, Log, TEXT("APNGBall::OnBeginOverlap actor %  SweepResult.Normal: %s  SweepResult.ImpactNormal: %s"), *OtherActor->GetName(), *SweepResult.Normal.ToString(), *SweepResult.ImpactNormal.ToString());
		FVector hitPlaneNormal = SweepResult.Normal * FVector(1, 1, 0);
		FVector reflection = mLastPush.Direction - FVector::DotProduct(hitPlaneNormal, mLastPush.Direction) * hitPlaneNormal * 2;
		float serverTime = GetServerTime();

		MulticastRPCUpdatePushData(GetActorLocation(), reflection.GetSafeNormal(), serverTime);
	}
}

void APNGBall::MulticastRPCUpdatePushData_Implementation(FVector Start, FVector Direction, float Time)
{
	FColor color = (GetWorld()->IsServer()) ? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, color, FString::Printf(TEXT("APNGBall::MulticastRPCUpdatePushData New direction %s"), *Direction.ToString()));

	mLastPush.StartLocation = Start;
	mLastPush.Direction = Direction;
	mLastPush.Time = Time;
}

#pragma optimize ("", off)