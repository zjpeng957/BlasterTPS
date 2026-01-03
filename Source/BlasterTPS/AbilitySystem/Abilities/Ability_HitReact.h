#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Ability_HitReact.generated.h"

class UAnimMontage;

UCLASS()
class BLASTERTPS_API UAbility_HitReact : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_HitReact();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Montage to play for hit react
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	UAnimMontage* HitReactMontage;

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
};
