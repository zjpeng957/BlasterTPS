// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterBotCharacter.h"
#include "AbilitySystemComponent.h"

ABlasterBotCharacter::ABlasterBotCharacter()
{
	// Create a local ASC for the bot (bots own their ASC instead of PlayerState)
	BotAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("BotAbilitySystemComponent"));
	if (BotAbilitySystemComponent)
	{
		BotAbilitySystemComponent->SetIsReplicated(true);
	}
}

void ABlasterBotCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize AttributeSet on bot using DefaultAttributeEffect if on server
	if (HasAuthority())
	{
		// Initialize Actor Info for the bot's ASC
		if (BotAbilitySystemComponent)
		{
			BotAbilitySystemComponent->InitAbilityActorInfo(this, this);
			// Apply default attributes if set
			if (DefaultAttributeEffect)
			{
				FGameplayEffectContextHandle EffectContext = BotAbilitySystemComponent->MakeEffectContext();
				EffectContext.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = BotAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, EffectContext);
				if (SpecHandle.IsValid())
				{
					BotAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), BotAbilitySystemComponent);
				}
			}
		}
	}
}

UAbilitySystemComponent* ABlasterBotCharacter::GetAbilitySystemComponent() const
{
	return BotAbilitySystemComponent;
}
