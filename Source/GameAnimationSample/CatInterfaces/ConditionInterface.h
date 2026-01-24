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
	void AddTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character|Conditions")
	void RemoveTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character|Conditions")
	bool HaseTag(const FGameplayTag& Tag);
};

