// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "BlasterPlayerState.generated.h"

class UAbilitySystemComponent;
class UBlasterAttributeSet;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class BLASTERTPS_API ABlasterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABlasterPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	// IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION()
	float GetHealth() const;

	UFUNCTION()
	float GetMaxHealth() const;

	UFUNCTION()
	void SetHealth(float NewHealth);

	UFUNCTION()
	void SetMaxHealth(float NewMaxHealth);

	UFUNCTION()
	float GetShield() const;

	UFUNCTION()
	float GetMaxShield() const;

	UFUNCTION()
	void SetShield(float NewShield);

	UFUNCTION()
	void SetMaxShield(float NewMaxShield);

	UFUNCTION()
	float GetMana() const;

	UFUNCTION()
	float GetMaxMana() const;

	UFUNCTION()
	void SetMana(float NewMana);

	UFUNCTION()
	void SetMaxMana(float NewMaxMana);

	// Initialize attributes via a GameplayEffect (call once on server when ASC initialized)
	void InitializeAttributes(TSubclassOf<UGameplayEffect> DefaultAttributeEffect);

	// Provide access to the AttributeSet owned by PlayerState
	UFUNCTION()
	UBlasterAttributeSet* GetAttributeSet() const { return AttributeSet; }

private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	// Gameplay Ability System component owned by PlayerState
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	// Attribute set (migrated from Character)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UBlasterAttributeSet* AttributeSet;

	bool bAttributesInitialized = false;
};
