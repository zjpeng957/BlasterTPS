// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAttributeSet.h"
#include "Net/UnrealNetwork.h"

UBlasterAttributeSet::UBlasterAttributeSet()
{
	// Initialize default values
	Health = 100.f;
	MaxHealth = 100.f;
	Shield = 100.f;
	MaxShield = 100.f;
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

void UBlasterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
}
