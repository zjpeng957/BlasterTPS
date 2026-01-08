#include "Ability_Melee.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BlasterTPS/Character/BlasterCharacter.h"
#include "BlasterTPS/Weapon/MeleeWeapon.h"
#include "AbilitySystemComponent.h"

UAbility_Melee::UAbility_Melee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAbility_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	// {
	// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	// 	return;
	// }

	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!AvatarPawn)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Play montage
	if (MeleeMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MeleeMontage);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &UAbility_Melee::OnMontageFinished);
			MontageTask->OnCancelled.AddDynamic(this, &UAbility_Melee::OnMontageFinished);
			MontageTask->OnInterrupted.AddDynamic(this, &UAbility_Melee::OnMontageFinished);
			MontageTask->OnBlendOut.AddDynamic(this, &UAbility_Melee::OnMontageFinished);
			MontageTask->ReadyForActivation();
		}
	}

	// Show Melee Weapon
	if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(AvatarPawn))
	{
		BlasterChar->SwitchMeleeWeaponVisibility(true);
		if (AMeleeWeapon* MW = BlasterChar->GetMeleeWeapon())
		{
			MW->ResetHitActors();
		}
	}

	// Wait for hit event
	if (HitEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitEventTag, nullptr, false);
		if (WaitTask)
		{
			WaitTask->EventReceived.AddDynamic(this, &UAbility_Melee::OnMeleeHitEvent);
			WaitTask->ReadyForActivation();
		}
	}
}

void UAbility_Melee::OnMeleeHitEvent(const FGameplayEventData Payload)
{
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!AvatarPawn) return;

	AMeleeWeapon* MeleeWeapon = nullptr;
	if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(AvatarPawn))
	{
		MeleeWeapon = BlasterChar->GetMeleeWeapon();
	}

	if (MeleeWeapon)
	{
		float Mag = (float)Payload.EventMagnitude;
		MeleeWeapon->PerformAttack(AvatarPawn, DamageGameplayEffect, Mag);
	}
}

void UAbility_Melee::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbility_Melee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Hide melee weapon
	if (APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(AvatarPawn))
		{
			BlasterChar->SwitchMeleeWeaponVisibility(false);
			if (AMeleeWeapon* MW = BlasterChar->GetMeleeWeapon())
			{
				MW->SetActorHiddenInGame(true);
			}
		}
	}

	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
