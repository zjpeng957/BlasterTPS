#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

class UGameplayEffect;

UCLASS()
class BLASTERTPS_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	AMeleeWeapon();

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void PerformAttack(AActor* InstigatorActor, TSubclassOf<UGameplayEffect> DamageEffectClass, float DamageAmountOverride = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void ResetHitActors();

protected:
	UPROPERTY(EditAnywhere, Category = "Melee")
	float TraceLength = 150.f;

	UPROPERTY(EditAnywhere, Category = "Melee")
	float TraceRadius = 40.f;

	UPROPERTY(EditAnywhere, Category = "Melee")
	int32 MaxTargetsPerSwing = 5;

	UPROPERTY(Transient)
	TArray<AActor*> ActorsHitThisSwing;
};
