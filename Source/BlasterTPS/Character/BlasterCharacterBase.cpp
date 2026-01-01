// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "BlasterTPS/Character/BlasterAttributeSet.h"
#include "BlasterTPS/PlayerState/BlasterPlayerState.h"

ABlasterCharacterBase::ABlasterCharacterBase()
{
	AbilitySystemComponent = nullptr;
	// AttributeSet moved to PlayerState; do not create it here
	bAttributesInitialized = false;
}

UAbilitySystemComponent* ABlasterCharacterBase::GetAbilitySystemComponent() const
{
	if (AbilitySystemComponent) return AbilitySystemComponent;
	if (const APlayerState* PS = GetPlayerState())
	{
		if (const ABlasterPlayerState* BPS = Cast<ABlasterPlayerState>(PS))
		{
			return BPS->GetAbilitySystemComponent();
		}
	}
	return nullptr;
}

UBlasterAttributeSet* ABlasterCharacterBase::GetAttributeSet() const
{
	if (const APlayerState* PS = GetPlayerState())
	{
		if (const ABlasterPlayerState* BPS = Cast<ABlasterPlayerState>(PS))
		{
			return BPS->GetAttributeSet();
		}
	}
	return nullptr;
}

void ABlasterCharacterBase::InitializeAttributes(TSubclassOf<UGameplayEffect> DefaultAttributeEffectClass)
{
	if (bAttributesInitialized) return;
	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (!DefaultAttributeEffectClass) return;
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DefaultAttributeEffectClass, 1.f, EffectContext);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
			bAttributesInitialized = true;
		}
	}
}
