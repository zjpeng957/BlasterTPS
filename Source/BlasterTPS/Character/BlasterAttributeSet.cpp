// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

UBlasterAttributeSet::UBlasterAttributeSet()
{
	// Initialize default values
	Health = 100.f;
	MaxHealth = 100.f;
	Shield = 100.f;
	MaxShield = 100.f;
	MoveSpeed = 600.f; // default Unreal walk speed
	JumpVelocity = 600.f; // default Unreal jump velocity
}

void UBlasterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp Health/Shield to Max values
	if (Attribute == GetMaxHealthAttribute())
	{
		// Ensure health doesn't exceed new max
		if (Health.GetCurrentValue() > NewValue)
		{
			Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, NewValue));
		}
	}
	if (Attribute == GetMaxShieldAttribute())
	{
		if (Shield.GetCurrentValue() > NewValue)
		{
			Shield.SetCurrentValue(FMath::Clamp(Shield.GetCurrentValue(), 0.f, NewValue));
		}
	}
	
	// If MoveSpeed or JumpVelocity changes, we might want to apply to CharacterMovement. This cannot be done here because we need an Actor context in OnRep or via listeners.
}

void UBlasterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, Health, OldHealth);
}

void UBlasterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, MaxHealth, OldMaxHealth);
}

void UBlasterAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, Shield, OldShield);
}

void UBlasterAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, MaxShield, OldMaxShield);
}

void UBlasterAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, MoveSpeed, OldMoveSpeed);
	// Sync to character movement component if possible
	if (AActor* OwningActor = Cast<AActor>(GetOwningActor()))
	{
		if (ACharacter* Char = Cast<ACharacter>(OwningActor))
		{
			if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
			{
				MoveComp->MaxWalkSpeed = MoveSpeed.GetCurrentValue();
			}
		}
	}
}

void UBlasterAttributeSet::OnRep_JumpVelocity(const FGameplayAttributeData& OldJumpVelocity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, JumpVelocity, OldJumpVelocity);
	// Sync to character movement component if possible
	if (AActor* OwningActor = Cast<AActor>(GetOwningActor()))
	{
		if (ACharacter* Char = Cast<ACharacter>(OwningActor))
		{
			if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
			{
				MoveComp->JumpZVelocity = JumpVelocity.GetCurrentValue();
			}
		}
	}
}

void UBlasterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, JumpVelocity, COND_None, REPNOTIFY_Always);
}
