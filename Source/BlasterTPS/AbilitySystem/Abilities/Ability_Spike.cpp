#include "Ability_Spike.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "BlasterTPS/Actor/SpikeActor.h"
#include "BlasterTPS/Actor/SpikeTarget.h"

UAbility_Spike::UAbility_Spike()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAbility_Spike::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 只检查消耗，不立即提交（不扣费、不冷却），等到确认放置时再提交
	if (!CheckCost(Handle, ActorInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Ability_Spike Activated. TargetActorClass: %s"), *GetNameSafe(TargetActorClass));

	if (!TargetActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetActorClass is NULL in Ability_Spike! Please set it in the Blueprint."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!TargetActorClass->IsChildOf(AGameplayAbilityTargetActor::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("TargetActorClass %s is not a subclass of AGameplayAbilityTargetActor!"), *GetNameSafe(TargetActorClass));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Check if it is ASpikeTarget
	if (!TargetActorClass->IsChildOf(ASpikeTarget::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActorClass %s is NOT a subclass of ASpikeTarget! Check your Blueprint settings."), *GetNameSafe(TargetActorClass));
	}

	AActor* OwnerActor = GetOwningActorFromActorInfo();
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	AGameplayAbilityTargetActor* TargetActor = GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(
		TargetActorClass, 
		FTransform::Identity, 
		OwnerActor, 
		AvatarPawn, 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn TargetActor!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetActor spawned: %s"), *GetNameSafe(TargetActor));

	TargetActor->FinishSpawning(FTransform::Identity);

	UE_LOG(LogTemp, Warning, TEXT("TargetActor FinishSpawning called."));

	// Create target actor for ground placement
	UAbilityTask_WaitTargetData* WaitTargetTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this, 
		FName("WaitTargetData"), 
		EGameplayTargetingConfirmation::UserConfirmed,
		TargetActor
	);

	if (WaitTargetTask)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitTargetTask created. Calling ReadyForActivation."));
		WaitTargetTask->ValidData.AddDynamic(this, &UAbility_Spike::OnTargetDataReady);
		WaitTargetTask->Cancelled.AddDynamic(this, &UAbility_Spike::OnTargetDataCancelled);
		WaitTargetTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create WaitTargetTask!"));
		TargetActor->Destroy(); // Clean up if task creation failed
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

	void UAbility_Spike::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data)
	{
		if (Data.Num() > 0)
		{
			if (const FGameplayAbilityTargetData* TargetData = Data.Get(0))
			{
				const FHitResult* HitResult = TargetData->GetHitResult();
				if (HitResult && HitResult->bBlockingHit)
				{
					// 确认放置，此时提交消耗和冷却
					if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
					{
						EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
						return;
					}

					FTransform SpawnTransform;
					SpawnTransform.SetLocation(HitResult->Location);
					SpawnTransform.SetRotation(FQuat::Identity);

					if (GetWorld())
					{
						AActor* OwnerActor = GetOwningActorFromActorInfo();
						APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
						
						ASpikeActor* Spike = GetWorld()->SpawnActorDeferred<ASpikeActor>(
							SpikeActorClass, 
							SpawnTransform, 
							OwnerActor, 
							InstigatorPawn, 
							ESpawnActorCollisionHandlingMethod::AlwaysSpawn
						);
						
						if (Spike)
						{
							Spike->FinishSpawning(SpawnTransform);
						}
					}
				}
			}
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

void UAbility_Spike::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
