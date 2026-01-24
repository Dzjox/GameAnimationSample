#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RailRidingComponent.generated.h"

class ARail;
class ACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API URailRidingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URailRidingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Rail")
	void StartRide(ARail* InRail);

	UFUNCTION(BlueprintCallable, Category="Rail")
	void StopRide();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Rail")
	FVector RailOffset = FVector(0,0,30);
	
protected:
	UPROPERTY()
	ACharacter* OwnerChar;
	UPROPERTY()
	ARail* Rail;

	UPROPERTY()
	float Distance = 0.f;
	UPROPERTY()
	float Speed = 0.f;
	UPROPERTY()
	float RailLength = 0.f;
	
	ACharacter* GetOwnerCharacter() const;
};

