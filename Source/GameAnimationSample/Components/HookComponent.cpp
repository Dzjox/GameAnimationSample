// Fill out your copyright notice in the Description page of Project Settings.


#include "HookComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "../Actors/AnchorPoint.h"

UHookComponent::UHookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHookComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	UpdateClosestAnchor();
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ClosestAnchorTimerHandle, this, &UHookComponent::UpdateClosestAnchor, 0.5f, true);
	}
}

void UHookComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ClosestAnchorTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void UHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsPulling || !OwnerCharacter || !TargetAnchor) return;

	FVector OwnerLoc = OwnerCharacter->GetActorLocation();
	FVector TargetLoc = TargetAnchor->GetActorLocation();
	FVector ToTarget = TargetLoc - OwnerLoc;
	float SquaredDist = ToTarget.SquaredLength();
	FVector Dir = ToTarget.GetSafeNormal();

	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	if (!MoveComp) return;

	if (SquaredDist > PullStopDistance * PullStopDistance)
	{
		if (MoveComp->MovementMode != MOVE_Flying)
		{
			PrevMovementMode = MoveComp->MovementMode;
			MoveComp->SetMovementMode(MOVE_Flying);
		}
		MoveComp->Velocity = Dir * PullSpeed;
	}
	else
	{
		StopPullAndLaunch();
	}
}

void UHookComponent::RegisterNearbyAnchor(AAnchorPoint* Anchor)
{
	if (!Anchor) return;
	if (NearbyAnchors.Contains(Anchor)) return;
	NearbyAnchors.Add(Anchor);
}

void UHookComponent::UnregisterNearbyAnchor(AAnchorPoint* Anchor)
{
	if (!Anchor) return;
	NearbyAnchors.Remove(Anchor);
}

AAnchorPoint* UHookComponent::FindClosestVisibleAnchor()
{
	UWorld* World = GetWorld();
	if (!World || !OwnerCharacter) return nullptr;

	AAnchorPoint* Best = nullptr;
	float BestDistSq = FLT_MAX;
	FVector Start = OwnerCharacter->GetActorLocation();

	for (AAnchorPoint* Anchor : NearbyAnchors)
	{
		if (!IsValid(Anchor)) continue;

		FVector End = Anchor->GetActorLocation();

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerCharacter);
		Params.AddIgnoredActor(Anchor);
		bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
		if (bHit) continue;

		float DistSq = FVector::DistSquared(Start, End);
		if (DistSq >= BestDistSq) continue;

		Best = Anchor;
		BestDistSq = DistSq;
	}

	return Best;
}

void UHookComponent::UpdateClosestAnchor()
{
	AAnchorPoint* NewClosest = FindClosestVisibleAnchor();
	if (NewClosest != ClosestVisibleAnchor)
	{
		ClosestVisibleAnchor = NewClosest;
		OnClosestAnchorChanged.Broadcast(NewClosest);
	}
}

bool UHookComponent::HookToClosestAnchor()
{
	if (!OwnerCharacter) OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return false;

	AAnchorPoint* Anchor = ClosestVisibleAnchor;
	if (!Anchor) return false;

	StartPullToAnchor(Anchor);
	return true;
}

void UHookComponent::StartPullToAnchor(AAnchorPoint* Anchor)
{
	TargetAnchor = Anchor;
	bIsPulling = true;
}

void UHookComponent::StopPullAndLaunch()
{
	if (!OwnerCharacter || !TargetAnchor) 
	{
		bIsPulling = false;
		TargetAnchor = nullptr;
		return;
	}

	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->SetMovementMode((EMovementMode)PrevMovementMode);
	}

	FVector FromAnchor =  TargetAnchor->GetActorLocation() - OwnerCharacter->GetActorLocation();
	FVector LaunchDir = FromAnchor.GetSafeNormal();
	if (LaunchDir.IsNearlyZero())
	{
		LaunchDir = OwnerCharacter->GetActorForwardVector();
	}

	FVector LaunchVel = LaunchDir * LaunchStrength + FVector::UpVector * LaunchUpwardStrength;
	OwnerCharacter->LaunchCharacter(LaunchVel, true, true);

	bIsPulling = false;
	TargetAnchor = nullptr;
}
