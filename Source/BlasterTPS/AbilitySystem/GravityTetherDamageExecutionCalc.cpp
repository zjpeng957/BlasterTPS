// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityTetherDamageExecutionCalc.h"

#include "AbilitySystemComponent.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"

void UGravityTetherDamageExecutionCalc::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float BaseDamage = CalculateBaseDamage(ExecutionParams, Spec, TargetTags, EvaluationParameters);
	
	float TetherCount = Spec.GetSetByCallerMagnitude(BlasterGameplayTags::SetByCaller::TetheredCount, false, 1.0f);
	
	float DamageToApply = BaseDamage * (1.0f + (TetherCount * 0.5f));
	FGameplayEffectContextHandle Context = ExecutionParams.GetOwningSpec().GetContext();
	// Context.AddHitResult(FHitResult(), FGameplayTag::RequestGameplayTag(FName("Data.DamageValue")), FinalDamage);
	// Context.AddHitResult(FGameplayTag::RequestGameplayTag(FName("Data.DamageValue")), FinalDamage);
	ApplyDamageToShieldAndHealth(ExecutionParams, OutExecutionOutput, EvaluationParameters, DamageToApply);
	
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (TargetASC)
	{
		// Build cue params
		FGameplayCueParameters CueParams;
		// Provide effect context so the cue knows source/instigator/etc
		CueParams.EffectContext = Spec.GetContext();

		// If you have a hitresult / location available, set it so cue can spawn at that world position.
		// Attempt to fetch hit result from effect context (may not exist depending on how Spec/Context was made)
		if (Spec.GetContext().GetHitResult())
		{
			CueParams.Location = Spec.GetContext().GetHitResult()->Location;
		}
		else
		{
			// Fallback: use target actor location if available
			AActor* TargetActor = Cast<AActor>(TargetASC->GetOwner());
			if (TargetActor)
			{
				CueParams.Location = TargetActor->GetActorLocation();
			}
		}
		
		// Hypothetical API — if your engine exposes RawMagnitude:
		CueParams.RawMagnitude = DamageToApply;

		// Ensure your damage cue tag exists (e.g., "GameplayCue.Damage")
		FGameplayTag DamageCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.DamageNumber"));

		// Execute the cue on the target ASC — this will invoke the GameplayCueNotify on clients
		TargetASC->ExecuteGameplayCue(DamageCueTag, CueParams); 
	}
}
