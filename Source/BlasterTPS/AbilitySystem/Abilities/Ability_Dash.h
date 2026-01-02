#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Ability_Dash.generated.h"

UCLASS()
class BLASTERTPS_API UAbility_Dash : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_Dash();

	// Ability config
	// Duration of sprint in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	float DashDuration = 0.7f;
	
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	float DashDistance = 200.f;
	
	// Optional montage to play while sprinting
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	UAnimMontage* DashMontage;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnDashFinished();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	TArray<TSubclassOf<UGameplayEffect>> DashEffects;
};
