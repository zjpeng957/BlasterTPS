// #pragma once
//
// #include "CoreMinimal.h"
// #include "Abilities/Tasks/AbilityTask.h"
// #include "AbilityTask_WaitProjectileHit.generated.h"
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitProjectileHitDelegate, const FHitResult&, HitResult);
//
// /**
//  * 等待指定 Projectile Actor 发生碰撞的 Ability Task
//  */
// UCLASS()
// class BLASTERTPS_API UAbilityTask_WaitProjectileHit : public UAbilityTask
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(BlueprintAssignable)
// 	FWaitProjectileHitDelegate OnHit;
//
// 	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
// 	static UAbilityTask_WaitProjectileHit* WaitProjectileHit(UGameplayAbility* OwningAbility, AActor* ProjectileActor);
//
// 	virtual void Activate() override;
// 	virtual void OnDestroy(bool AbilityEnded) override;
//
// protected:
// 	UPROPERTY()
// 	AActor* ProjectileActor;
//
// 	UFUNCTION()
// 	void OnProjectileHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
// };
//
