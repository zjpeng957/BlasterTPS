// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HItScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERTPS_API AHItScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;
};
