// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JumpPanel.generated.h"

class UBoxComponent;
class UArrowComponent;
class UStaticMeshComponent;
class USplineComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AJumpPanel : public AActor
{
	GENERATED_BODY()

public:
	AJumpPanel();

	UPROPERTY(EditAnywhere)
	float LaunchStrength = 1500.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float LaunchZMultiplier = 1.0f;

	UPROPERTY(EditAnywhere)
	bool bShowTrajectory = true;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "2"))
	int32 TrajectoryPoints = 30;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01"))
	float TrajectoryTimeStep = 0.1f;

	UPROPERTY(EditAnywhere)
	FRotator LaunchDirection = FRotator(90,0,0);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* TrajectorySpline;

	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                           bool bFromSweep, const FHitResult& SweepResult);

	void UpdateTrajectorySpline();
	void UpdateArrowDirection();

	// helper to compute final launch velocity from LaunchDirection / LaunchStrength / LaunchZMultiplier
	FVector GetLaunchVelocity() const;
};
