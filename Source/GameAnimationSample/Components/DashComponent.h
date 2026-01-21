// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"

class ACharacter;
struct FTimerHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDash);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChargesChanged, int32, NewCharges, int32, RechargeTime);

UCLASS(ClassGroup=(Custom), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dash")
	bool Dash();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	int32 MaxCharges = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	int32 CurrentCharges = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float UseDelaySeconds = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float RegenIntervalSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float ForwardStrength = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float UpwardStrength = 200.0f;

	UPROPERTY(BlueprintAssignable, Category = "Dash")
	FOnDash OnDash;

	UPROPERTY(BlueprintAssignable, Category = "Dash")
	FOnChargesChanged OnChargesChanged;

private:
	bool bCanDash = true;

	FTimerHandle UseDelayTimerHandle;
	FTimerHandle RegenTimerHandle;

	void OnUseDelayComplete();
	void OnRegenTick();

	UPROPERTY()
	ACharacter* OwnerCharacterCache;
	ACharacter* GetOwnerCharacter();
};
