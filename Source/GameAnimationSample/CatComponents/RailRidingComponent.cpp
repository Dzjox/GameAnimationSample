#include "RailRidingComponent.h"
#include "GameplayTagContainer.h"
#include "../CatActors/Rail.h"
#include "../CatInterfaces/ConditionInterface.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

URailRidingComponent::URailRidingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URailRidingComponent::StopRide()
{
	if (GetOwnerCharacter())
	{
		if (UCharacterMovementComponent* MoveComp = GetOwnerCharacter()->GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
			MoveComp->GravityScale = 1.f;
		}

		IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsRailing")));

		if (UWorld* World = GetOwnerCharacter()->GetWorld())
		{
			FTimerDelegate Del = FTimerDelegate::CreateLambda([Owner = GetOwnerCharacter()]()
			{
				if (Owner && Owner->GetClass()->ImplementsInterface(UConditionInterface::StaticClass()))
				{
					IConditionInterface::Execute_RemoveTag(Owner, FGameplayTag::RequestGameplayTag(FName("Character.Condition.Block.Railing")));
				}
			});
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, Del, 0.5f, false);
		}
	}

	Rail = nullptr;
}

ACharacter* URailRidingComponent::GetOwnerCharacter() const
{
	if (OwnerChar) return OwnerChar;
	return Cast<ACharacter>(GetOwner());
}

void URailRidingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IConditionInterface::Execute_HaseTag(
		GetOwnerCharacter(),
		FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsRailing"))))
	{
		return;
	}
	
	if (!Rail || !Rail->Spline || !GetOwnerCharacter()) 
	{
		if (IConditionInterface::Execute_HaseTag(
			GetOwnerCharacter(),
			FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsRailing"))))
		{
			StopRide();
		}
		return;
	}
	
	Distance += Speed * DeltaTime;

	if (RailLength > 0.f && Distance >= RailLength)
	{
		StopRide();
		return;
	}

	FVector NewLocation = Rail->Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	FRotator NewRotation = Rail->Spline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	GetOwnerCharacter()->SetActorLocationAndRotation(NewLocation + RailOffset, NewRotation);
}

void URailRidingComponent::StartRide(ARail* InRail)
{
	if (!GetOwnerCharacter() || !InRail) return;
	if (IConditionInterface::Execute_HaseTag(
		GetOwnerCharacter(),
		FGameplayTag::RequestGameplayTag(FName("Character.Condition.Block.Railing")))
		) return;

	IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.Block.Railing")));
	IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsRailing")));
	
	Rail = InRail;
	Distance = Rail->Spline->GetDistanceAlongSplineAtLocation(GetOwnerCharacter()->GetActorLocation(), ESplineCoordinateSpace::World);
	Speed = Rail->DefaultSpeed;
	
	if (InRail->Spline)
	{
		RailLength = InRail->Spline->GetSplineLength();
	}
	else
	{
		RailLength = 0.f;
	}

	if (UCharacterMovementComponent* MoveComp = GetOwnerCharacter()->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_Flying);
		MoveComp->GravityScale = 0.f;
	}

}
