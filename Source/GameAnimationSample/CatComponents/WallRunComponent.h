// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WallRunComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UWallRunComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category="WallRunComponent")
	float WallRunGravityScale = 0.5f;

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category="WallRunComponent")
	float TraceLengthWallDetection = 60;
	
	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category="WallRunComponent")
	float StartLaunchFactor;

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category="WallRunComponent")
	float WallRunSpeed;

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category="WallRunComponent")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category="WallRunComponent")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

protected:
	UFUNCTION()
	ACharacter* GetOwnerCharacter() const;

	UFUNCTION()
	bool GetWallNormal(FVector& WallNormalOut, bool& IsRightSideOut);

	UFUNCTION()
	void StartWallRun();

	UFUNCTION()
	void StopWallRun();

	UPROPERTY()
	ACharacter* OwnerChar;
	UPROPERTY()
	FVector WallNormal;
	UPROPERTY()
	bool IsRightSide;
	UPROPERTY()
	double MinSpeedForWallRun;
	UPROPERTY()
	float StartWallRunSpeed;
	UPROPERTY()
	float DelayBeforeCanWallRunAgain = 1.f;
	

	FTimerHandle BlockTagRemovalTimer;
};
