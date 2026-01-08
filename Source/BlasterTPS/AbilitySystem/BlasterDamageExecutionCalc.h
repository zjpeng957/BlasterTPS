#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BlasterDamageExecutionCalc.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERTPS_API UBlasterDamageExecutionCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UBlasterDamageExecutionCalc();
	float CalculateBaseDamage(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                          const FGameplayEffectSpec& Spec, const FGameplayTagContainer* TargetTags,
	                          FAggregatorEvaluateParameters EvaluationParameters) const;
	void ApplyDamageToShieldAndHealth(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                  FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	                                  FAggregatorEvaluateParameters EvaluationParameters, float DamageToApply) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

