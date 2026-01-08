// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterDamageExecutionCalc.h"
#include "GravityTetherDamageExecutionCalc.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERTPS_API UGravityTetherDamageExecutionCalc : public UBlasterDamageExecutionCalc
{
	GENERATED_BODY()
	
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
