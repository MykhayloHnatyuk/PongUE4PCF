#include "PNGPawnMain.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PNGPlayerControllerMain.h"

APNGPawnMain::APNGPawnMain()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APNGPawnMain::BeginPlay()
{
	Super::BeginPlay();
	
	SetupView();
}

void APNGPawnMain::InitializeBP(UStaticMeshComponent* PawnMesh)
{
	mMesh = PawnMesh;
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

void APNGPawnMain::SetupView()
{
	ensure(mMesh);

	auto controller = GetController();

	if(controller && controller->IsLocalPlayerController())
	{
		// Set up our player view.
	}
	else
	{
		// Set up enemy player view.
		auto enemyMaterial = UMaterialInstanceDynamic::Create(EnemyViewMaterial, this);
		mMesh->SetMaterial(0, enemyMaterial);
	}	
}
