// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rail.generated.h"

class USceneComponent;
class USplineComponent;
class UStaticMeshComponent;
class UBoxComponent;
class ACharacter;

UCLASS()
class GAMEANIMATIONSAMPLE_API ARail : public AActor
{
	GENERATED_BODY()

public:
	ARail();

	UPROPERTY(VisibleAnywhere)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, Category = "Rail", meta = (ClampMin = "1.0"))
	float DefaultSpeed = 1000.f;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh_Example;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision_Example;

	UPROPERTY(EditAnywhere, Category = "Rail", meta = (ClampMin = "1.0"))
	float SpawnPeriod = 50.f;

	UPROPERTY(EditAnywhere, Category = "Rail")
	FName GeneratedTag = FName("Rail_Generated");

	UFUNCTION()
	void RebuildRailFromExamples();

	void ClearGeneratedExamples();

public:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                            bool bFromSweep, const FHitResult& SweepResult);
};
