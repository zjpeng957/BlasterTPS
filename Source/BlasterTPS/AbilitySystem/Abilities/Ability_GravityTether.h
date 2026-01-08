#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Ability_GravityTether.generated.h"

class AGravityCore;
class UGameplayEffect;

/**
 * 产生预瞄抛物线，确认后发射 GravityCore 的技能
 */
UCLASS()
class BLASTERTPS_API UAbility_GravityTether : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_GravityTether();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AGravityCore> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class AGameplayAbilityTargetActor> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
TSubclassOf<UGameplayEffect> ExplosionEffectClass;

	// Configurable Params for Tether
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tether Config")
	float TetherRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tether Config")
	float TetherStrength = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tether Config")
	float TetherDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tether Config")
	int32 MaxTetherTargets = 5;

	// Effect to apply to tethered actors (adds State.Tethered tag)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tether Config")
	TSubclassOf<UGameplayEffect> TetherStatusEffectClass;

	// Tag for the explode input event
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tether Config")
	FGameplayTag ExplodeInputTag;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_ApplyMultiTargetPullForce> PullTask;
	
	UPROPERTY()
	TWeakObjectPtr<AGravityCore> Projectile;
	
	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnGravityCoreHit(const FHitResult& HitResult);

	UFUNCTION()
	void OnTetherForceApplied(const TArray<AActor*>& AffectedActors);

	UFUNCTION()
	void OnExplodeTriggered(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnTetherFinished();
};
