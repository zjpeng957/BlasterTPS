#include "Ability_GravityTether.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "BlasterTPS/Actor/GravityCore.h"
#include "BlasterTPS/AbilitySystem/TargetActor/GravityTetherTargetActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BlasterTPS/AbilitySystem/Tasks/AbilityTask_WaitProjectileHit.h"
#include "BlasterTPS/AbilitySystem/Tasks/AbilityTask_ApplyMultiTargetPullForce.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UAbility_GravityTether::UAbility_GravityTether()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ExplodeInputTag = BlasterGameplayTags::Input::TetherSecPress;
}

void UAbility_GravityTether::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CheckCost(Handle, ActorInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ProjectileClass || !TargetActorClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Add Tethering state tag
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(BlasterGameplayTags::State::Tethering);

	// 此时不 commit (不扣除 Mana/Cooldown)，直到玩家确认目标
	
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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TargetActor->FinishSpawning(FTransform::Identity);

	// 创建等待目标的任务
	UAbilityTask_WaitTargetData* WaitTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		FName("WaitTarget"),
		EGameplayTargetingConfirmation::UserConfirmed,
		TargetActor
	);

	if (WaitTask)
	{
		WaitTask->ValidData.AddDynamic(this, &UAbility_GravityTether::OnTargetDataReady);
		WaitTask->Cancelled.AddDynamic(this, &UAbility_GravityTether::OnTargetDataCancelled);
		WaitTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UAbility_GravityTether::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data)
{
	// 玩家确认发射，此时提交资源 (消耗 Mana,计算 Cooldown)
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FVector TargetLocation = FVector::ZeroVector;
	FVector ValidSpawnLocation = FVector::ZeroVector;
	if (const FGameplayAbilityTargetData* TargetData = Data.Get(0))
	{
		if (const FHitResult* Hit = TargetData->GetHitResult())
		{
			TargetLocation = Hit->Location;
			ValidSpawnLocation = Hit->TraceStart;
		}
	}

	// 准备生成逻辑
	if (GetWorld())
	{
		AActor* OwningActor = GetOwningActorFromActorInfo();
		APawn* OwningPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
		
		if (OwningPawn)
		{
			FTransform SpawnTransform = OwningPawn->GetActorTransform();
			
			// Use the SpawnLocation from TargetActor if available to ensure consistency
			FVector SpawnLocation;
			if (!ValidSpawnLocation.IsZero())
			{
				SpawnLocation = ValidSpawnLocation;
			}
			else
			{
				// Fallback calculation
				SpawnLocation = SpawnTransform.GetLocation() + (SpawnTransform.GetRotation().GetForwardVector() * 100.f);
			}
			SpawnTransform.SetLocation(SpawnLocation);

			// 延迟生成以设置参数
			Projectile = GetWorld()->SpawnActorDeferred<AGravityCore>(
				ProjectileClass,
				SpawnTransform,
				OwningActor,
				OwningPawn,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

			if (Projectile.IsValid())
			{
				// 创建伤害 Spec
				if (DamageEffectClass)
				{
					FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
					// 如果需要 SetByCaller 设置伤害数值，可以在这里操作
					// DamageSpecHandle.Data->SetSetByCallerMagnitude(...)
					
					Projectile->DamageEffectSpecHandle = DamageSpecHandle;
				}
				
				// Calculate parabolic velocity to hit the target
				float OverrideGravityZ = 0.f;
				if(Projectile->ProjectileMovement)
				{
					OverrideGravityZ = GetWorld()->GetGravityZ() * Projectile->ProjectileMovement->ProjectileGravityScale;
				}

				FVector TossVelocity;
				bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
					this,
					TossVelocity,
					SpawnLocation,
					TargetLocation,
					OverrideGravityZ,
					0.5f
				);
				
				if (bHaveSolution)
				{
					// Align spawn rotation with velocity to prevent conflict during FinishSpawning
					// (FinishSpawning applies SpawnTransform rotation to the actor)
					SpawnTransform.SetRotation(TossVelocity.Rotation().Quaternion());
				}
				else
				{
					// Fallback to straight line if no solution
					FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
					TossVelocity = LaunchDirection * Projectile->ProjectileMovement->InitialSpeed;
					SpawnTransform.SetRotation(TossVelocity.Rotation().Quaternion());
				}
				Projectile->OnProjectileHit.AddDynamic(this, &UAbility_GravityTether::OnGravityCoreHit);
				Projectile->FinishSpawning(SpawnTransform);
				
				// Re-apply velocity after FinishSpawning to ensure it persists vs any initialization logic
				if (Projectile->ProjectileMovement)
				{
					Projectile->ProjectileMovement->Velocity = TossVelocity;
				}
			}
		}
	}
}

void UAbility_GravityTether::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(BlasterGameplayTags::State::Tethering);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbility_GravityTether::OnGravityCoreHit(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit && HitResult.GetActor())
	{
		// Spawn location for tethering from the hit location
		FVector CenterLoc = HitResult.Location;

		PullTask = UAbilityTask_ApplyMultiTargetPullForce::ApplyMultiTargetPullForce(
			this,
			CenterLoc,
			TetherRadius,
			TetherStrength,
			TetherDuration,
			MaxTetherTargets
		);

		if (PullTask)
		{
			PullTask->OnForceApplied.AddDynamic(this, &UAbility_GravityTether::OnTetherForceApplied);
			PullTask->OnFinished.AddDynamic(this, &UAbility_GravityTether::OnTetherFinished);
			PullTask->ReadyForActivation();
			
			// Ability will wait for task to finish via callback
		}
		else
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
		
		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ExplodeInputTag, nullptr, false);
		if (WaitEventTask)
		{
			WaitEventTask->EventReceived.AddDynamic(this, &UAbility_GravityTether::OnExplodeTriggered);
			WaitEventTask->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UAbility_GravityTether::OnTetherForceApplied(const TArray<AActor*>& AffectedActors)
{
	if (!TetherStatusEffectClass) return;

	for (AActor* Actor : AffectedActors)
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			// Apply "State.Tethered" tag via GE
			FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			
			FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(TetherStatusEffectClass, GetAbilityLevel(), ContextHandle);
			if (SpecHandle.IsValid())
			{
				// Usually "State.Tethered" GE is Infinite/Duration. If Duration, set it to match TetherDuration.
				// If the GE blueprint already has duration set, this might override or we assume it matches.
				// Best practice: Set duration dynamically.
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Duration")), TetherDuration);
				
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void UAbility_GravityTether::OnExplodeTriggered(FGameplayEventData Payload)
{
	int32 TargetCnt = PullTask->GetAffectedActors().Num();
	for (auto Actor: PullTask->GetAffectedActors())
	{
		if (!IsValid(Actor)) continue;
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			
			FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ExplosionEffectClass, GetAbilityLevel(), ContextHandle);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(BlasterGameplayTags::SetByCaller::TetheredCount, TargetCnt);
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	if (IsValid(PullTask)) PullTask->EndTask();
	if (Projectile.IsValid()) Projectile->Destroy();

	// Remove Tethering state tag
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(BlasterGameplayTags::State::Tethering);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbility_GravityTether::OnTetherFinished()
{
	// Remove Tethering state tag
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(BlasterGameplayTags::State::Tethering);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
