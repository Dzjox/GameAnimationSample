// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CatInterfaces/ConditionInterface.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConditionsChanged, FGameplayTagContainer&, Conditions);

UCLASS()
class GAMEANIMATIONSAMPLE_API ACharacterBase : public ACharacter, public IConditionInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	UPROPERTY(BlueprintAssignable, Category="Character|Events")
	FOnConditionsChanged OnConditionsChanged;

	UFUNCTION(BlueprintCallable, Category="Character|Conditions")
	bool HasCondition(const FGameplayTag& ConditionTag) const;

	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void Custom_AddMovementInput(FVector2D Input);

	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void Custom_AddControllerInput(FVector2D Input);

	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void Custom_Jump();

	virtual void AddCondition_Implementation(const FGameplayTag& ConditionTag) override;
	virtual void RemoveCondition_Implementation(const FGameplayTag& ConditionTag) override;

private:
	FGameplayTagContainer CharacterConditions;
};
