#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GravityCore.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitProjectileHitDelegate, const FHitResult&, HitResult);

UCLASS()
class BLASTERTPS_API AGravityCore : public AActor
{
	GENERATED_BODY()
	
public:	
	AGravityCore();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* GravityVFXComponent;

	// 由 Ability 传递进来的伤害句柄
	UPROPERTY(BlueprintReadWrite, Category = "GAS")
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	FWaitProjectileHitDelegate OnProjectileHit;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Tick(float DeltaTime) override;
	// virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Removed EndPlay override as it was for Cue cleanup

private:
	// bool bIsGravityActive = false; // Unused if tick is empty
	// float GravityRadius = 1000.f;  // These might be unused now if logic is in AbilityTask, but preserving class structure if needed
	// float GravityStrength = 3000000.f; 
};
