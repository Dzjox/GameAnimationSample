// Fill out your copyright notice in the Description page of Project Settings.


#include "WallRunComponent.h"

#include "GameAnimationSample/CatInterfaces/ConditionInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"



UWallRunComponent::UWallRunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWallRunComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IConditionInterface::Execute_HaseTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsWallRunning"))))
	{
		FVector Direction = UKismetMathLibrary::GreaterGreater_VectorRotator(WallNormal, IsRightSide? FRotator(0,90,0) : FRotator(0,-90,0));

		DrawDebugDirectionalArrow(GetWorld(), GetOwnerCharacter()->GetActorLocation(), GetOwnerCharacter()->GetActorLocation() + Direction * 200, 5, FColor::Yellow, false, -1, 0, 2);
		DrawDebugDirectionalArrow(GetWorld(), GetOwnerCharacter()->GetActorLocation(), GetOwnerCharacter()->GetActorLocation() + WallNormal * 200, 5, FColor::Red, false, -1, 0, 2);
		

		GetOwnerCharacter()->LaunchCharacter(Direction * WallRunSpeed * DeltaTime, false, false);
		
		if ( !GetWallNormal(WallNormal, IsRightSide)
			|| GetOwnerCharacter()->GetMovementComponent()->IsMovingOnGround())
		{
			StopWallRun();
		}
	}
	else
	{
		IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun.Right")));
		IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun.Left")));

		if (IConditionInterface::Execute_HaseTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Block.WallRunning"))))
		{
			return;
		}
		
		// Check for speed, if it's too low, stop wall run
		//if (UKismetMathLibrary::VSizeXYSquared(GetOwnerCharacter()->GetVelocity())>UKismetMathLibrary::Square(MinSpeedForWallRun))
		if (true)
		{
			if (GetWallNormal(WallNormal, IsRightSide))
			{
				if (IsRightSide)
				{
					IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun.Right")));
				}
				else
				{
					IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun.Left")));
				}
			}

			if (IConditionInterface::Execute_HaseTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun"))))
			{
				StartWallRun();
			}
		}
	}
}

ACharacter* UWallRunComponent::GetOwnerCharacter() const
{
	if (OwnerChar) return OwnerChar;
	return Cast<ACharacter>(GetOwner());
}

bool UWallRunComponent::GetWallNormal(FVector& WallNormalOut, bool& IsRightSideOut)
{
	FHitResult HitResult;
	FVector NeWallNormal = FVector::ZeroVector; // Chaika asked to name it like this

	// Right side check
	if (UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		GetOwnerCharacter()->GetActorLocation(),
		GetOwnerCharacter()->GetActorLocation() + GetOwnerCharacter()->GetActorRightVector() * TraceLengthWallDetection,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		DrawDebugType,
		HitResult,
		true))
	{
		WallNormalOut = HitResult.ImpactNormal;
		IsRightSideOut = true;
		return true;
	}

	// Left side check
	if (UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		GetOwnerCharacter()->GetActorLocation(),
		GetOwnerCharacter()->GetActorLocation() - GetOwnerCharacter()->GetActorRightVector() * TraceLengthWallDetection,
		ObjectTypes,
		false,
		TArray<AActor*>(),
		DrawDebugType,
		HitResult,
		true))
	{
		WallNormalOut = HitResult.ImpactNormal;
		IsRightSideOut = false;
		return true;
	}

	return false;
}

void UWallRunComponent::StartWallRun()
{
	IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun.Right")));
	IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Permit.WallRun.Left")));

	if (IsRightSide)
	{
		IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsWallRunning.Right")));
	}
	else
	{
		IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsWallRunning.Left")));
	}

	IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Block.Movement")));
	GetOwnerCharacter()->GetCharacterMovement()->GravityScale = WallRunGravityScale;
	StartWallRunSpeed = GetOwnerCharacter()->GetCharacterMovement()->Velocity.Size2D();

	FVector Direction = UKismetMathLibrary::GreaterGreater_VectorRotator(WallNormal, IsRightSide? FRotator(0,90,0) : FRotator(0,-90,0));
	Direction+= FVector(0,0,0.3f);
	GetOwnerCharacter()->LaunchCharacter(Direction * StartLaunchFactor, true, true);
}

void UWallRunComponent::StopWallRun()
{
	IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsWallRunning.Left")));
	IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Condition.IsWallRunning.Right")));
	IConditionInterface::Execute_RemoveTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Block.Movement")));

	GetOwnerCharacter()->GetCharacterMovement()->GravityScale = 1.f;
	WallNormal = FVector::ZeroVector;

	IConditionInterface::Execute_AddTag(GetOwnerCharacter(), FGameplayTag::RequestGameplayTag(FName("Character.Block.WallRunning")));
	
	if (UWorld* World = GetWorld())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			if (ACharacter* Character = GetOwnerCharacter())
			{
				IConditionInterface::Execute_RemoveTag(Character, FGameplayTag::RequestGameplayTag(FName("Character.Block.WallRunning")));
			}
		});
		
		World->GetTimerManager().SetTimer(BlockTagRemovalTimer, TimerDelegate, DelayBeforeCanWallRunAgain, false);
	}
}
