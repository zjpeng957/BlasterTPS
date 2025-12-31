// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "BlasterBotCharacter.generated.h"

class UAbilitySystemComponent;

UCLASS()
class BLASTERTPS_API ABlasterBotCharacter : public ABlasterCharacterBase
{
	GENERATED_BODY()

public:
	ABlasterBotCharacter();

	// IAbilitySystemInterface override - bot owns its ASC
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Ability System Component owned by this bot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* BotAbilitySystemComponent;
};
