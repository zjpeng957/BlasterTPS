#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Ability_Spike.generated.h"

class ASpikeActor;
class AGameplayAbilityTargetActor;

UCLASS()
class BLASTERTPS_API UAbility_Spike : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_Spike();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spike")
	TSubclassOf<ASpikeActor> SpikeActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spike")
	TSubclassOf<AGameplayAbilityTargetActor> TargetActorClass;

	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);
};
