// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HookComponent.generated.h"

class ACharacter;
class AAnchorPoint;
struct FTimerHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClosestAnchorChanged, AAnchorPoint*, NewAnchor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHookComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Hook")
	bool HookToClosestAnchor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
	float PullSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
	float PullStopDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
	float LaunchStrength = 1800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
	float LaunchUpwardStrength = 400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hook")
	AAnchorPoint* ClosestVisibleAnchor = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Hook")
	FOnClosestAnchorChanged OnClosestAnchorChanged;

	void RegisterNearbyAnchor(AAnchorPoint* Anchor);
	void UnregisterNearbyAnchor(AAnchorPoint* Anchor);

private:
	bool bIsPulling = false;
	UPROPERTY()
	AAnchorPoint* TargetAnchor = nullptr;
	UPROPERTY()
	ACharacter* OwnerCharacter = nullptr;
	uint8 PrevMovementMode = 0;

	FTimerHandle ClosestAnchorTimerHandle;

	UPROPERTY(Transient)
	TArray<AAnchorPoint*> NearbyAnchors;

	AAnchorPoint* FindClosestVisibleAnchor();
	void UpdateClosestAnchor();
	void StartPullToAnchor(AAnchorPoint* Anchor);
	void StopPullAndLaunch();
};
