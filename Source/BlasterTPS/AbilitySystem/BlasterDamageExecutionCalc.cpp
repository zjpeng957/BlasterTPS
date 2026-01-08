#include "BlasterDamageExecutionCalc.h"
#include "BlasterTPS/Character/BlasterAttributeSet.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"
#include "AbilitySystemComponent.h"

struct BlasterDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Shield);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxShield);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);

	BlasterDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBlasterAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBlasterAttributeSet, Shield, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBlasterAttributeSet, MaxShield, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBlasterAttributeSet, IncomingDamage, Target, true); // Snapshot? Usually false for damage calculation if we want modifiers.
		// Actually, if we want to capture the value AFTER modifiers, we use false (don't snapshot).
		// But IncomingDamage is usually modified by the GE itself.
		// If we use false, we get the value at execution time.
	}
};

static const BlasterDamageStatics& DamageStatics()
{
	static BlasterDamageStatics DStatics;
	return DStatics;
}

UBlasterDamageExecutionCalc::UBlasterDamageExecutionCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().ShieldDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxShieldDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
}

float UBlasterDamageExecutionCalc::CalculateBaseDamage(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FGameplayTagContainer* TargetTags, FAggregatorEvaluateParameters EvaluationParameters) const
{
	// Get Damage SetByCaller Magnitude
	float Damage = 0.f;
	// Assuming damage is passed via SetByCaller with the tag Data.Damage
	Damage = Spec.GetSetByCallerMagnitude(BlasterGameplayTags::SetByCaller::Damage, false, 0.0f);

	// Also capture IncomingDamage attribute magnitude (for non-SetByCaller damage)
	float IncomingDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().IncomingDamageDef, EvaluationParameters, IncomingDamage);
	
	// Combine damages. Note: SetByCaller damage was passed as negative in BlasterCharacter.cpp, but here we handle sign.
	// If SetByCaller is negative, we flip it. IncomingDamage is usually positive (magnitude).
	
	float DamageToApply = 0.f;
	if (Damage < 0)
	{
		DamageToApply = -Damage;
	}
	else
	{
		DamageToApply = Damage;
	}
	
	// Add IncomingDamage (assuming it's positive magnitude)
	DamageToApply += FMath::Max<float>(IncomingDamage, 0.f);

	// Check for Invincible tag
	if (TargetTags->HasTag(BlasterGameplayTags::State::Invincible))
	{
		DamageToApply = 0.f;
	}
	return DamageToApply;
}

void UBlasterDamageExecutionCalc::ApplyDamageToShieldAndHealth(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput, FAggregatorEvaluateParameters EvaluationParameters, float DamageToApply) const
{
	float CurrentShield = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ShieldDef, EvaluationParameters, CurrentShield);
	CurrentShield = FMath::Max<float>(CurrentShield, 0.0f);

	float CurrentHealth = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, EvaluationParameters, CurrentHealth);
	CurrentHealth = FMath::Max<float>(CurrentHealth, 0.0f);

	float DamageDoneToShield = 0.f;
	float DamageDoneToHealth = 0.f;

	if (CurrentShield > 0.f)
	{
		float ShieldDamage = FMath::Min(CurrentShield, DamageToApply);
		DamageDoneToShield = ShieldDamage;
		DamageToApply -= ShieldDamage;
	}

	DamageDoneToHealth = DamageToApply;

	if (DamageDoneToShield > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().ShieldProperty, EGameplayModOp::Additive, -DamageDoneToShield));
	}

	if (DamageDoneToHealth > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -DamageDoneToHealth));
	}
}

void UBlasterDamageExecutionCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float DamageToApply = CalculateBaseDamage(ExecutionParams, Spec, TargetTags, EvaluationParameters);
	
	ApplyDamageToShieldAndHealth(ExecutionParams, OutExecutionOutput, EvaluationParameters, DamageToApply);
}
