// Fill out your copyright notice in the Description page of Project Settings.


#include "HookComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "../CatActors/AnchorPoint.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"

UHookComponent::UHookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHookComponent::NotifyAnchorReached(AAnchorPoint* Anchor)
{
	if (!bIsPulling) return;
	if (!Anchor) return;
	if (TargetAnchor != Anchor) return;
	StopPullAndLaunch();
}

void UHookComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
		if (OwnerCapsule)
		{
			OwnerCapsule->OnComponentBeginOverlap.AddDynamic(this, &UHookComponent::OnCapsuleBeginOverlap);
			OwnerCapsule->OnComponentEndOverlap.AddDynamic(this, &UHookComponent::OnCapsuleEndOverlap);
		}
	}
	UpdateClosestAnchor();
}

void UHookComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwnerCapsule)
	{
		OwnerCapsule->OnComponentBeginOverlap.RemoveDynamic(this, &UHookComponent::OnCapsuleBeginOverlap);
		OwnerCapsule->OnComponentEndOverlap.RemoveDynamic(this, &UHookComponent::OnCapsuleEndOverlap);
		OwnerCapsule = nullptr;
	}
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ClosestAnchorTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void UHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateClosestAnchor();

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
		float EffectivePullSpeed = TargetAnchor ? TargetAnchor->PullSpeed : 0.f;
		MoveComp->Velocity = Dir * EffectivePullSpeed;
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
	float BestDistScore = FLT_MAX;
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector Forward = OwnerCharacter->GetActorForwardVector().GetSafeNormal();

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

		FVector ToAnchor = (End - Start).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToAnchor);
		float ClampedDot = FMath::Clamp(Dot, 0.0f, 1.0f);
		float Weight = 1.0f - LookDirectionMultiplier * ClampedDot;
		float DistScore = DistSq * Weight;

		if (DistScore >= BestDistScore) continue;

		Best = Anchor;
		BestDistScore = DistScore;
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

	float PreservedSpeed = OwnerCharacter->GetVelocity().Size();

	if (TargetAnchor)
	{
		TargetAnchor->LaunchCharacterFromAnchor(OwnerCharacter, PreservedSpeed);
	}

	bIsPulling = false;
	TargetAnchor = nullptr;
}

void UHookComponent::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor && OtherComp) OtherActor = OtherComp->GetOwner();
	AAnchorPoint* Anchor = Cast<AAnchorPoint>(OtherActor);
	if (!Anchor && OtherComp) Anchor = Cast<AAnchorPoint>(OtherComp->GetOwner());
	if (Anchor) RegisterNearbyAnchor(Anchor);
}

void UHookComponent::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor && OtherComp) OtherActor = OtherComp->GetOwner();
	AAnchorPoint* Anchor = Cast<AAnchorPoint>(OtherActor);
	if (!Anchor && OtherComp) Anchor = Cast<AAnchorPoint>(OtherComp->GetOwner());
	if (Anchor) UnregisterNearbyAnchor(Anchor);
}
