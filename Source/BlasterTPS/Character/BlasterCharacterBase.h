// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterTPS/Interface/InteractWithCrosshairsInterface.h"
#include "AbilitySystemInterface.h"
#include "BlasterCharacterBase.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UBlasterAttributeSet;

UCLASS(Abstract)
class BLASTERTPS_API ABlasterCharacterBase : public ACharacter, public IInteractWithCrosshairsInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacterBase();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Default attributes gameplay effect (set in Blueprint / defaults)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	// Initialize attributes via a GameplayEffect (call once on server when ASC initialized)
	void InitializeAttributes(TSubclassOf<UGameplayEffect> DefaultAttributeEffect);

	UFUNCTION()
	UBlasterAttributeSet* GetAttributeSet() const;

protected:
	// AbilitySystemComponent is now owned by PlayerState; Character will retrieve it from PlayerState at runtime.
	// Cache pointer after initialization
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	// Attribute set moved to PlayerState; Character will access it via PlayerState

	bool bAttributesInitialized = false;
};
