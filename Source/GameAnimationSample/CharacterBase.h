// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	ACharacterBase();

	UFUNCTION(BlueprintCallable, Category="Character|Conditions")
	void AddCondition(const FGameplayTag& ConditionTag);

	UFUNCTION(BlueprintCallable, Category="Character|Conditions")
	void RemoveCondition(const FGameplayTag& ConditionTag);

	UFUNCTION(BlueprintCallable, Category="Character|Conditions")
	bool HasCondition(const FGameplayTag& ConditionTag) const;

	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void Custom_AddMovementInput(FVector2D Input);

	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void Custom_AddControllerInput(FVector2D Input);

	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void Custom_Jump();

	virtual void Tick(float DeltaSeconds) override;

private:
	FGameplayTagContainer CharacterConditions;

};
