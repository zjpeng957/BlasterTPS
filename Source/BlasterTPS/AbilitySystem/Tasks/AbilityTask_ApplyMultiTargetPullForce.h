#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ApplyMultiTargetPullForce.generated.h"

class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiTargetPullDelegate, const TArray<AActor*>&, AffectedActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMultiTargetPullFinishedDelegate);

/**
 * Task to apply a radial pull force (Root Motion) to multiple characters near a location.
 * Best used on Server only for affecting other characters.
 */
UCLASS()
class BLASTERTPS_API UAbilityTask_ApplyMultiTargetPullForce : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FMultiTargetPullDelegate OnForceApplied;

	UPROPERTY(BlueprintAssignable)
	FMultiTargetPullFinishedDelegate OnFinished;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_ApplyMultiTargetPullForce* ApplyMultiTargetPullForce(
		UGameplayAbility* OwningAbility, 
		FVector CenterLocation, 
		float Radius, 
		float Strength, 
		float Duration, 
		int32 MaxTargets);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	TArray<TObjectPtr<AActor>> GetAffectedActors() const {return AffectedActors;}

protected:
	FVector CenterLocation;
	float Radius;
	float Strength;
	float Duration;
	int32 MaxTargets;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> AffectedActors;

	struct FAppliedForceInfo
	{
		TWeakObjectPtr<UCharacterMovementComponent> MovementComponent;
		uint16 RootMotionSourceID;
	};

	TArray<FAppliedForceInfo> AppliedForces;

	void OnDurationFinished();
};
