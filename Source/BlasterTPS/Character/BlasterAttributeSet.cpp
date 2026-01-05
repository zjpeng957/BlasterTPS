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
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"

namespace
{
	static UAbilitySystemComponent* GetASCFromActor(AActor* Actor)
	{
		if (!Actor)
		{
			return nullptr;
		}

		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
		{
			return ASI->GetAbilitySystemComponent();
		}

		if (UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>())
		{
			return ASC;
		}

		if (APawn* Pawn = Cast<APawn>(Actor))
		{
			if (APlayerState* PS = Pawn->GetPlayerState())
			{
				return GetASCFromActor(PS);
			}
		}

		if (AController* Controller = Cast<AController>(Actor))
		{
			if (APlayerState* PS = Controller->PlayerState)
			{
				return GetASCFromActor(PS);
			}
		}

		return nullptr;
	}

	// Resolve the real attacker ASC from GameplayEffectContext.
	// We prefer EffectCauser/Instigator over OriginalInstigatorAbilitySystemComponent because the latter
	// can legitimately be the Target ASC for some application paths, which breaks "attacker != target" checks.
	static UAbilitySystemComponent* ResolveAttackerASC(const FGameplayEffectModCallbackData& Data)
	{
		const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();

		if (AActor* Causer = Context.GetEffectCauser())
		{
			if (UAbilitySystemComponent* ASC = GetASCFromActor(Causer))
			{
				return ASC;
			}
		}

		if (AActor* Instigator = Context.GetOriginalInstigator())
		{
			if (UAbilitySystemComponent* ASC = GetASCFromActor(Instigator))
			{
				return ASC;
			}
		}

		if (UAbilitySystemComponent* OriginalASC = Context.GetOriginalInstigatorAbilitySystemComponent())
		{
			return OriginalASC;
		}

		// Fallback: try Target's instigator pawn (covers some non-ability damage paths)
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			if (AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get())
			{
				if (APawn* InstigatorPawn = TargetActor->GetInstigator())
				{
					if (UAbilitySystemComponent* ASC = GetASCFromActor(InstigatorPawn))
					{
						return ASC;
					}
				}
			}
		}

		return nullptr;
	}
}

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
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Shield called. New Value: %.1f. ASC: %s"), Shield.GetCurrentValue(), GetOwningAbilitySystemComponent() ? *GetOwningAbilitySystemComponent()->GetName() : TEXT("NULL"));
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

	UAbilitySystemComponent* TargetASC = (Data.Target.AbilityActorInfo.IsValid() ? Data.Target.AbilityActorInfo->AbilitySystemComponent.Get() : nullptr);
	UAbilitySystemComponent* AttackerASC = ResolveAttackerASC(Data);

	// Target context (used for death/elimination)
	AController* TargetController = nullptr;
	ABlasterCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ABlasterCharacter>(Data.Target.AbilityActorInfo->AvatarActor.Get());
	}

	// Source controller (used for elimination credit)
	AController* SourceController = nullptr;
	if (AttackerASC && AttackerASC->AbilityActorInfo.IsValid())
	{
		SourceController = AttackerASC->AbilityActorInfo->PlayerController.Get();

		// Fallback: if no PC (AI or non-player pawn), derive controller from avatar
		if (SourceController == nullptr && AttackerASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			if (APawn* Pawn = Cast<APawn>(AttackerASC->AbilityActorInfo->AvatarActor.Get()))
			{
				SourceController = Pawn->GetController();
			}
		}
	}

	auto TryRestoreManaToAttacker = [&](float DamageDealt)
	{
		if (DamageDealt <= 0.f)
		{
			return;
		}
		if (!AttackerASC || !TargetASC)
		{
			return;
		}

		// Don't restore mana for self-damage / environmental cases where attacker can't be resolved distinctly.
		if (AttackerASC == TargetASC)
		{
			return;
		}

		if (!ManaRestoreEffectClass)
		{
			return;
		}

		FGameplayEffectContextHandle EffectContext = AttackerASC->MakeEffectContext();
		EffectContext.AddSourceObject(AttackerASC->GetAvatarActor());
		FGameplayEffectSpecHandle SpecHandle = AttackerASC->MakeOutgoingSpec(ManaRestoreEffectClass, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(BlasterGameplayTags::SetByCaller::Mana, DamageDealt);
			AttackerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	};

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		// Restore Mana to attacker equal to damage dealt (Health part)
		const float DamageDealt = -Data.EvaluatedData.Magnitude;
		TryRestoreManaToAttacker(DamageDealt);

		if (TargetCharacter && GetHealth() <= 0.f && !TargetCharacter->IsElimmed())
		{
			// Target died
			if (ABlasterGameMode* GM = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				ABlasterPlayerController* TargetPC = Cast<ABlasterPlayerController>(TargetController);
				ABlasterPlayerController* AttackerPC = Cast<ABlasterPlayerController>(SourceController);
				GM->PlayerEliminated(TargetCharacter, TargetPC, AttackerPC);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));

		// Restore Mana to attacker equal to damage dealt (Shield part)
		const float DamageDealt = -Data.EvaluatedData.Magnitude;
		TryRestoreManaToAttacker(DamageDealt);
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			// This logic is now handled by ExecutionCalculation, but if we have direct modifiers to IncomingDamage
			// that are NOT handled by ExecutionCalculation (e.g. simple GEs), we might want to apply them here.
			// However, ExecutionCalculation runs BEFORE PostGameplayEffectExecute and it modifies Health/Shield directly.
			// So if IncomingDamage is modified by a GE that uses the ExecutionCalculation, the ExecutionCalculation
			// will have already consumed the value (from the spec) and applied changes to Health/Shield.
			// But wait, ExecutionCalculation captures attributes. If IncomingDamage is a backing attribute,
			// does the GE modify it first?
			// Yes, if the GE has a modifier for IncomingDamage, that modifier is applied.
			// BUT ExecutionCalculation is usually used to REPLACE the standard application logic for specific attributes?
			// No, ExecutionCalculation is an additional step.
			
			// If the GE has a modifier for IncomingDamage, and also has an ExecutionCalculation that captures IncomingDamage.
			// The ExecutionCalculation runs. It reads the value of IncomingDamage (which includes the modifier).
			// It then outputs modifiers for Health/Shield.
			// Then the GE's modifiers are applied. So IncomingDamage is modified on the AttributeSet.
			// So here in PostGameplayEffectExecute, IncomingDamage will have a value.
			// We should just consume it (reset to 0) because the damage has already been applied to Health/Shield by the ExecutionCalculation.
			
			// Wait, if the GE *only* modifies IncomingDamage and relies on PostGameplayEffectExecute to apply damage,
			// then we would do the logic here.
			// But we are using ExecutionCalculation.
			// So we just need to reset it.
		}
	}
}
