#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Ability_Melee.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class BLASTERTPS_API UAbility_Melee : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_Melee();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	UAnimMontage* MeleeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	FGameplayTag HitEventTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UFUNCTION()
	void OnMeleeHitEvent(const FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;
};

