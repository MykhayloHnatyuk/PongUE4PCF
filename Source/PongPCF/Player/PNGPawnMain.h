// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PNGPawnMain.generated.h"

class UStaticMeshComponent;

UCLASS()
class PONGPCF_API APNGPawnMain : public APawn
{
	GENERATED_BODY()

public:

	APNGPawnMain();

	float GetMovementSpeed() const { return MovementSpeed; }

protected:

	virtual void BeginPlay() override;

	// Should be called in BP to initialize proper mesh.
	UFUNCTION(BlueprintCallable)
	void InitializeBP(UStaticMeshComponent* PawnMesh);

	UFUNCTION()
	void OnGameScoreChangedHandler(int Player1Score, int Player2Score);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|PlayerParams")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|PlayerParams")
	float DefaultPlayerWidth;

	// Pawn scale (width) might be decreased during the game. This is a limit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pong|PlayerParams")
	float MinPlayerWidth;

	UPROPERTY(EditAnywhere, Category = "Pong|Player View")
	UMaterialInterface* EnemyViewMaterial;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Limits OutTargetedLocation if it will hit something on attempt to change location.
	void LimitMoveToLocation(FVector& OutTargetedLocation) const;

	UStaticMeshComponent* GetMesh() const { return mMesh; }

private:

	void SetMesh(UStaticMeshComponent* Mesh) { mMesh = Mesh; }

	// Used to setup player's view to make it clear which player you are in control of.
	void SetupView();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRPCUpdatePawnSize(float sizeRatio);

	UFUNCTION(Reliable, NetMulticast)
	void MulticastRPCUpdatePawnSize(float sizeRatio);

	// The more we score, the smaller we get.
	void UpdatePawnSize(float sizeRatio);

	// Main mesh of our pawn.
	UStaticMeshComponent* mMesh;
};
