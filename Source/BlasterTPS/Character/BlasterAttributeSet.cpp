// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "BlasterTPS/Character/BlasterCharacter.h"
#include "BlasterTPS/PlayerController/BlasterPlayerController.h"
#include "BlasterTPS/GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"

UBlasterAttributeSet::UBlasterAttributeSet()
{
	// Initialize default values
	Health = 100.f;
	MaxHealth = 100.f;
	Shield = 100.f;
	MaxShield = 100.f;
	Mana = 100.f;
	MaxMana = 100.f;
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
	if (Attribute == GetMaxManaAttribute())
	{
		if (Mana.GetCurrentValue() > NewValue)
		{
			Mana.SetCurrentValue(FMath::Clamp(Mana.GetCurrentValue(), 0.f, NewValue));
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

void UBlasterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, Mana, OldMana);
}

void UBlasterAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBlasterAttributeSet, MaxMana, OldMaxMana);
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
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBlasterAttributeSet, JumpVelocity, COND_None, REPNOTIFY_Always);
}

void UBlasterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ABlasterCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ABlasterCharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	ABlasterCharacter* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<ABlasterCharacter>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<ABlasterCharacter>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if (TargetCharacter && GetHealth() <= 0.f && !TargetCharacter->IsElimmed())
		{
			// Target died
			if (ABlasterGameMode* GM = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				// We need BlasterPlayerController for the eliminated player
				ABlasterPlayerController* TargetPC = Cast<ABlasterPlayerController>(TargetController);
				// We need BlasterPlayerController for the attacker
				ABlasterPlayerController* AttackerPC = Cast<ABlasterPlayerController>(SourceController);

				GM->PlayerEliminated(TargetCharacter, TargetPC, AttackerPC);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
}
