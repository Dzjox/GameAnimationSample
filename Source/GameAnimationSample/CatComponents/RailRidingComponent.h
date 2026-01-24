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
	void StartRide(ARail* InRail, float StartDistance, float InSpeed);

	UFUNCTION(BlueprintCallable, Category="Rail")
	void StopRide();

private:
	TWeakObjectPtr<ARail> Rail;
	float Distance = 0.f;
	float Speed = 0.f;
	bool bIsRiding = false;
	float RailLength = 0.f;
};

