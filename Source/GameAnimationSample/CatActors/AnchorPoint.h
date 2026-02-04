// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnchorPoint.generated.h"

class USceneComponent;
class USphereComponent;
class UHookComponent;
class UArrowComponent;
class USplineComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API AAnchorPoint : public AActor
{
	GENERATED_BODY()

public:
	AAnchorPoint();

	UPROPERTY(VisibleAnywhere, Category = "Anchor")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Anchor")
	USphereComponent* DetectionSphere;

	UPROPERTY(EditAnywhere, Category = "Anchor")
	float DetectionRadius = 9000.f;

	UPROPERTY(EditAnywhere, Category = "Anchor")
	float PullSpeed = 9000.f;

	UPROPERTY(VisibleAnywhere, Category = "Anchor")
	UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere, Category = "Anchor")
	USplineComponent* TrajectorySpline;

	UPROPERTY(EditAnywhere, Category = "Anchor|Launch")
	FRotator LaunchDirection = FRotator(-30.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Anchor|Launch")
	float LaunchStrength = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Anchor|Launch")
	float LaunchUpwardStrength = 300.f;

	UPROPERTY(EditAnywhere, Category = "Anchor|Launch")
	bool bShowTrajectory = true;

	UPROPERTY(EditAnywhere, Category = "Anchor|Launch", meta = (ClampMin = "2"))
	int32 TrajectoryPoints = 20;

	UPROPERTY(EditAnywhere, Category = "Anchor|Launch", meta = (ClampMin = "0.01"))
	float TrajectoryTimeStep = 0.1f;

	UPROPERTY(VisibleAnywhere, Category = "Anchor")
	USphereComponent* LaunchSphere;

	UPROPERTY(EditAnywhere, Category = "Anchor")
	float ArrivalRadius = 150.f;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable, Category = "Anchor")
	void LaunchCharacterFromAnchor(ACharacter* Character, float PreservedSpeed = 0.f);

protected:
	void UpdateDetectionSphereRadius();
	void UpdateTrajectorySpline();
	void UpdateArrowDirection();
	void UpdateLaunchSphereRadius();

	UFUNCTION()
	void OnLaunchSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
