// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnchorPoint.generated.h"

class USceneComponent;
class USphereComponent;
class UHookComponent;

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
	float DetectionRadius = 1000.f;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
