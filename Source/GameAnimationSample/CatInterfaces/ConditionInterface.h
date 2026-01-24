#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ConditionInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UConditionInterface : public UInterface
{
	GENERATED_BODY()
};

class IConditionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character|Conditions")
	void AddCondition(const FGameplayTag& ConditionTag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character|Conditions")
	void RemoveCondition(const FGameplayTag& ConditionTag);
};

