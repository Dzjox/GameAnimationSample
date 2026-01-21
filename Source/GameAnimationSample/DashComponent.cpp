#include "DashComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACharacter* UDashComponent::GetOwnerCharacter()
{
	if (!IsValid(OwnerCharacterCache))
	{
		OwnerCharacterCache = Cast<ACharacter>(GetOwner());
	}
	return OwnerCharacterCache;
}

bool UDashComponent::Dash()
{
	if (!bCanDash) return false;
	if (CurrentCharges <= 0) return false;

	ACharacter* Char = GetOwnerCharacter();
	if (!Char) return false;

	const FVector Forward = Char->GetActorForwardVector();
	const FVector LaunchVel = Forward * ForwardStrength + FVector::UpVector * UpwardStrength;

	Char->LaunchCharacter(LaunchVel, true, true);

	CurrentCharges = FMath::Max(0, CurrentCharges - 1);
	bCanDash = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(UseDelayTimerHandle, this, &UDashComponent::OnUseDelayComplete, UseDelaySeconds, false);
	}

	if (GetWorld() && CurrentCharges < MaxCharges)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(RegenTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UDashComponent::OnRegenTick, RegenIntervalSeconds, true);
		}
	}

	return true;
}

void UDashComponent::OnUseDelayComplete()
{
	bCanDash = true;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UseDelayTimerHandle);
	}
}

void UDashComponent::OnRegenTick()
{
	if (CurrentCharges < MaxCharges)
	{
		CurrentCharges = FMath::Clamp(CurrentCharges + 1, 0, MaxCharges);
	}

	if (CurrentCharges >= MaxCharges && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
	}
}
