#include "RailRidingComponent.h"
#include "../CatActors/Rail.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URailRidingComponent::URailRidingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetComponentTickEnabled(true);
}

void URailRidingComponent::StartRide(ARail* InRail, float StartDistance, float InSpeed)
{
	if (!InRail) return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	Rail = InRail;
	Distance = FMath::Max(0.f, StartDistance);
	Speed = InSpeed > 0.f ? InSpeed : 300.f;
	bIsRiding = true;

	if (InRail->Spline)
	{
		RailLength = InRail->Spline->GetSplineLength();
	}
	else
	{
		RailLength = 0.f;
	}

	if (UCharacterMovementComponent* MoveComp = OwnerChar->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_Flying);
		MoveComp->GravityScale = 0.f;
	}
}

void URailRidingComponent::StopRide()
{
	if (!bIsRiding) return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar)
	{
		if (UCharacterMovementComponent* MoveComp = OwnerChar->GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
			MoveComp->GravityScale = 1.f;
		}
	}

	bIsRiding = false;
	Rail = nullptr;

	DestroyComponent();
}

void URailRidingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsRiding) return;
	ARail* RailPtr = Rail.Get();
	if (!RailPtr || !RailPtr->Spline) 
	{
		StopRide();
		return;
	}

	Distance += Speed * DeltaTime;

	if (RailLength > 0.f && Distance >= RailLength)
	{
		StopRide();
		return;
	}

	FVector NewLocation = RailPtr->Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator NewRotation = RailPtr->Spline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		OwnerActor->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

