// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"


ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ACharacterBase::HasCondition(const FGameplayTag& ConditionTag) const
{
	return CharacterConditions.HasTag(ConditionTag);
}

void ACharacterBase::Custom_AddMovementInput(FVector2D Input)
{
	if (CharacterConditions.HasTag(FGameplayTag::RequestGameplayTag(FName("Character.Condition.Block.Movement")))) return;
	FVector Input3D = FVector(Input.X, Input.Y, 0.f);
	FRotator ControlRotationZ = FRotator(0,GetControlRotation().Yaw, 0);
	AddMovementInput(ControlRotationZ.RotateVector(Input3D));
}

void ACharacterBase::Custom_AddControllerInput(FVector2D Input)
{
	if (CharacterConditions.HasTag(FGameplayTag::RequestGameplayTag(FName("Character.Condition.Block.Rotation")))) return;
	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

void ACharacterBase::Custom_Jump()
{
	if (CharacterConditions.HasTag(FGameplayTag::RequestGameplayTag(FName("Character.Condition.Block.Jump")))) return;
	Jump();
}

void ACharacterBase::AddTag_Implementation(const FGameplayTag& Tag)
{
	if (!CharacterConditions.HasTag(Tag))
	{
		CharacterConditions.AddTag(Tag);
		OnConditionsChanged.Broadcast(CharacterConditions);
	}
}

void ACharacterBase::RemoveTag_Implementation(const FGameplayTag& Tag)
{
	if (CharacterConditions.HasTag(Tag))
	{
		CharacterConditions.RemoveTag(Tag);
		OnConditionsChanged.Broadcast(CharacterConditions);
	}
}

bool ACharacterBase::HaseTag_Implementation(const FGameplayTag& Tag)
{
	return CharacterConditions.HasTag(Tag);
}

