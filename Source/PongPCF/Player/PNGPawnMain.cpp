#include "PNGPawnMain.h"
#include "DrawDebugHelpers.h"

APNGPawnMain::APNGPawnMain()
{
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

void APNGPawnMain::LimitMoveToLocation(FVector& OutTargetedLocation) const
{
	const FVector currentLocation = GetActorLocation(); 

	if(currentLocation == OutTargetedLocation)
	{
		return;
	}

	const FVector direction = (OutTargetedLocation - currentLocation).GetSafeNormal();
	float halfLength = GetComponentsBoundingBox(false).GetExtent().X;
	const FVector traceEndPoint = OutTargetedLocation + direction * halfLength;

	FHitResult hitResult;
	if(GetWorld()->LineTraceSingleByChannel(hitResult, currentLocation, traceEndPoint, ECollisionChannel::ECC_GameTraceChannel11))
	{
		OutTargetedLocation = hitResult.Location - direction * halfLength;
	}
}
