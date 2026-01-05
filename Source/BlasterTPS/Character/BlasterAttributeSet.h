// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BlasterAttributeSet.generated.h"

class UGameplayEffect;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class BLASTERTPS_API UBlasterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBlasterAttributeSet();

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, Health)

	// Max Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, MaxHealth)

	// Shield
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Attributes")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, Shield)

	// Max Shield
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Attributes")
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, MaxShield)

	// Mana
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, Mana)

	// Max Mana
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, MaxMana)

	// Movement speed (MaxWalkSpeed)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Attributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, MoveSpeed)

	// Jump velocity (JumpZVelocity)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_JumpVelocity, Category = "Attributes")
	FGameplayAttributeData JumpVelocity;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, JumpVelocity)

	// Meta Attribute for Damage
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UBlasterAttributeSet, IncomingDamage)

	// UAttributeSet overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Shield(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	virtual void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	virtual void OnRep_JumpVelocity(const FGameplayAttributeData& OldJumpVelocity);

	// Gameplay Effect to use for restoring Mana when damage is dealt.
	// If set, this GE will be applied with SetByCaller magnitude for Mana.
	// The GE should have a modifier for Mana using SetByCaller (Tag: Data.Damage or similar? No, we need a specific tag for Mana Restore Amount).
	// Or we can just use the same tag "Data.Damage" and interpret it as "Amount" in the GE?
	// Let's use a new tag "Data.ManaRestore" or just reuse "Data.Damage" if convenient, but "Data.ManaRestore" is cleaner.
	// However, for simplicity, I will assume the GE uses "Data.Damage" (as magnitude) or I will define a new tag.
	// Actually, I can just use "Data.Damage" tag for the magnitude, as it represents the damage dealt.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> ManaRestoreEffectClass;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
