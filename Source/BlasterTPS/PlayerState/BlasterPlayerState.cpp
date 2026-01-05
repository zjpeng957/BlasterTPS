// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "BlasterTPS/Character/BlasterCharacter.h"
#include "BlasterTPS/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "BlasterTPS/Character/BlasterAttributeSet.h"

ABlasterPlayerState::ABlasterPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create AttributeSet here so PlayerState owns it
	AttributeSet = CreateDefaultSubobject<UBlasterAttributeSet>(TEXT("AttributeSet"));
}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

UAbilitySystemComponent* ABlasterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float ABlasterPlayerState::GetHealth() const
{
	// Prefer Character's AttributeSet if available
	if (const ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet()) return Char->GetAttributeSet()->Health.GetCurrentValue();
	}
	if (AttributeSet) return AttributeSet->Health.GetCurrentValue();
	return 0.f;
}

float ABlasterPlayerState::GetMaxHealth() const
{
	if (const ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet()) return Char->GetAttributeSet()->MaxHealth.GetCurrentValue();
	}
	if (AttributeSet) return AttributeSet->MaxHealth.GetCurrentValue();
	return 0.f;
}

void ABlasterPlayerState::SetHealth(float NewHealth)
{
	if (ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet())
		{
			Char->GetAttributeSet()->Health.SetCurrentValue(NewHealth);
			return;
		}
	}
	if (AttributeSet)
	{
		AttributeSet->Health.SetCurrentValue(NewHealth);
	}
}

void ABlasterPlayerState::SetMaxHealth(float NewMaxHealth)
{
	if (ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet())
		{
			Char->GetAttributeSet()->MaxHealth.SetCurrentValue(NewMaxHealth);
			if (Char->GetAttributeSet()->Health.GetCurrentValue() > NewMaxHealth)
			{
				Char->GetAttributeSet()->Health.SetCurrentValue(NewMaxHealth);
			}
			return;
		}
	}
	if (AttributeSet)
	{
		AttributeSet->MaxHealth.SetCurrentValue(NewMaxHealth);
		if (AttributeSet->Health.GetCurrentValue() > NewMaxHealth)
		{
			AttributeSet->Health.SetCurrentValue(NewMaxHealth);
		}
	}
}

float ABlasterPlayerState::GetShield() const
{
	if (const ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet()) return Char->GetAttributeSet()->Shield.GetCurrentValue();
	}
	if (AttributeSet) return AttributeSet->Shield.GetCurrentValue();
	return 0.f;
}

float ABlasterPlayerState::GetMaxShield() const
{
	if (const ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet()) return Char->GetAttributeSet()->MaxShield.GetCurrentValue();
	}
	if (AttributeSet) return AttributeSet->MaxShield.GetCurrentValue();
	return 0.f;
}

void ABlasterPlayerState::SetShield(float NewShield)
{
	if (ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet())
		{
			Char->GetAttributeSet()->Shield.SetCurrentValue(NewShield);
			return;
		}
	}
	if (AttributeSet)
	{
		AttributeSet->Shield.SetCurrentValue(NewShield);
	}
}

void ABlasterPlayerState::SetMaxShield(float NewMaxShield)
{
	if (ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet())
		{
			Char->GetAttributeSet()->MaxShield.SetCurrentValue(NewMaxShield);
			if (Char->GetAttributeSet()->Shield.GetCurrentValue() > NewMaxShield)
			{
				Char->GetAttributeSet()->Shield.SetCurrentValue(NewMaxShield);
			}
			return;
		}
	}
	if (AttributeSet)
	{
		AttributeSet->MaxShield.SetCurrentValue(NewMaxShield);
		if (AttributeSet->Shield.GetCurrentValue() > NewMaxShield)
		{
			AttributeSet->Shield.SetCurrentValue(NewMaxShield);
		}
	}
}

float ABlasterPlayerState::GetMana() const
{
	if (const ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet()) return Char->GetAttributeSet()->Mana.GetCurrentValue();
	}
	if (AttributeSet) return AttributeSet->Mana.GetCurrentValue();
	return 0.f;
}

float ABlasterPlayerState::GetMaxMana() const
{
	if (const ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet()) return Char->GetAttributeSet()->MaxMana.GetCurrentValue();
	}
	if (AttributeSet) return AttributeSet->MaxMana.GetCurrentValue();
	return 0.f;
}

void ABlasterPlayerState::SetMana(float NewMana)
{
	if (ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet())
		{
			Char->GetAttributeSet()->Mana.SetCurrentValue(NewMana);
			return;
		}
	}
	if (AttributeSet)
	{
		AttributeSet->Mana.SetCurrentValue(NewMana);
	}
}

void ABlasterPlayerState::SetMaxMana(float NewMaxMana)
{
	if (ABlasterCharacter* Char = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (Char->GetAttributeSet())
		{
			Char->GetAttributeSet()->MaxMana.SetCurrentValue(NewMaxMana);
			if (Char->GetAttributeSet()->Mana.GetCurrentValue() > NewMaxMana)
			{
				Char->GetAttributeSet()->Mana.SetCurrentValue(NewMaxMana);
			}
			return;
		}
	}
	if (AttributeSet)
	{
		AttributeSet->MaxMana.SetCurrentValue(NewMaxMana);
		if (AttributeSet->Mana.GetCurrentValue() > NewMaxMana)
		{
			AttributeSet->Mana.SetCurrentValue(NewMaxMana);
		}
	}
}

void ABlasterPlayerState::InitializeAttributes(TSubclassOf<UGameplayEffect> DefaultAttributeEffect)
{
	if (bAttributesInitialized) return;
	if (!HasAuthority()) return;
	if (!AbilitySystemComponent) return;
	if (!DefaultAttributeEffect) return;

	// Use the GameplayEffect class to create a spec
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, EffectContext);
	if (SpecHandle.IsValid())
	{
		// Apply to AbilitySystemComponent; since the AttributeSet is owned by this PlayerState and registered with the ASC,
		// the values in AttributeSet will be modified and replicated to clients.
		AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
		bAttributesInitialized = true;
	}
}

