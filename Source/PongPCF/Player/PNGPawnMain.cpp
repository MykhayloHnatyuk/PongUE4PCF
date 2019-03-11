#include "PNGPawnMain.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PNGPlayerControllerMain.h"
#include "Settings/PNGGameStateMain.h"
#include "Settings/PNGGameModeMain.h"

APNGPawnMain::APNGPawnMain()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APNGPawnMain::BeginPlay()
{
	Super::BeginPlay();
	
	SetupView();

	APNGGameStateMain* gs = Cast<APNGGameStateMain>(GetWorld()->GetGameState());
	gs->OnGameScoreChanged().RemoveAll(this);
	gs->OnGameScoreChanged().AddUObject(this, &APNGPawnMain::OnGameScoreChangedHandler);
}

void APNGPawnMain::InitializeBP(UStaticMeshComponent* PawnMesh)
{
	SetMesh(PawnMesh);
}

void APNGPawnMain::OnGameScoreChangedHandler(int Player1Score, int Player2Score)
{
	// We want to change pawn size depending on a current score.
	if(IsLocallyControlled())
	{
		auto controller = Cast<APNGPlayerControllerMain>(GetController());
		int newScore = controller->IsPlayerOne() ? Player1Score : Player2Score;
		float scoreToSizeRatio = float(newScore) / TARGET_GAME_SCORE_TO_WIN;

		// Now let's update size on a local machine and than send it to others.
		if(HasAuthority())
		{
			MulticastRPCUpdatePawnSize(scoreToSizeRatio);
		}
		else
		{
			UpdatePawnSize(scoreToSizeRatio);
			ServerRPCUpdatePawnSize(scoreToSizeRatio);
		}
	}	
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
		// We hit something that should make us stop.
		OutTargetedLocation = hitResult.Location - direction * halfLength;
	}
}

void APNGPawnMain::SetupView()
{
	auto mesh = GetMesh();
	ensure(mesh);

	auto controller = GetController();

	if(controller && controller->IsLocalPlayerController())
	{
		// Set up our player view.
		// Will be default for now.
	}
	else
	{
		// Set up enemy player view.
		auto enemyMaterial = UMaterialInstanceDynamic::Create(EnemyViewMaterial, this);
		mesh->SetMaterial(0, enemyMaterial);
	}	
}

void APNGPawnMain::MulticastRPCUpdatePawnSize_Implementation(float sizeRatio)
{
	UpdatePawnSize(sizeRatio);
}

void APNGPawnMain::ServerRPCUpdatePawnSize_Implementation(float sizeRatio)
{
	UpdatePawnSize(sizeRatio);
}

bool APNGPawnMain::ServerRPCUpdatePawnSize_Validate(float sizeRatio)
{
	return true;
}

void APNGPawnMain::UpdatePawnSize(float sizeRatio)
{
	auto mesh = GetMesh();

	FVector scale = mesh->GetRelativeTransform().GetScale3D();
	scale.Y = FMath::Lerp(DefaultPlayerWidth, MinPlayerWidth, sizeRatio);
	mesh->SetRelativeScale3D(scale);
}