#include "AbilityTask_ApplyMultiTargetPullForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"

UAbilityTask_ApplyMultiTargetPullForce* UAbilityTask_ApplyMultiTargetPullForce::ApplyMultiTargetPullForce(
	UGameplayAbility* OwningAbility, 
	FVector CenterLocation, 
	float Radius, 
	float Strength, 
	float Duration, 
	int32 MaxTargets)
{
	UAbilityTask_ApplyMultiTargetPullForce* MyTask = NewAbilityTask<UAbilityTask_ApplyMultiTargetPullForce>(OwningAbility);
	MyTask->CenterLocation = CenterLocation;
	MyTask->Radius = Radius;
	MyTask->Strength = Strength;
	MyTask->Duration = Duration;
	MyTask->MaxTargets = MaxTargets;
	return MyTask;
}

void UAbilityTask_ApplyMultiTargetPullForce::Activate()
{
	Super::Activate();

	// Applying Root Motion to other characters must be done on Server
	if (!GetOwnerActor() || !GetOwnerActor()->HasAuthority())
	{
		// On client, we just wait for duration
		if (Duration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]() 
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_ApplyMultiTargetPullForce::OnDurationFinished, Duration, false);
			});
		}
		else
		{
			EndTask();
		}
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(Radius);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActor());

	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		CenterLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		CollisionShape,
		Params
	);

	if (bHit)
	{
		int32 TargetsCount = 0;
		for (const FOverlapResult& Result : OverlapResults)
		{
			if (TargetsCount >= MaxTargets)
			{
				break;
			}

			ACharacter* TargetChar = Cast<ACharacter>(Result.GetActor());
			// Only affect characters that are not the owner
			// TODO: Add Team Check here if needed
			if (TargetChar && TargetChar != GetAvatarActor())
			{
				UCharacterMovementComponent* MCC = TargetChar->GetCharacterMovement();
				if (MCC)
				{
					TSharedPtr<FRootMotionSource_RadialForce> AnyRadialForce = MakeShared<FRootMotionSource_RadialForce>();
					AnyRadialForce->InstanceName = FName("MultiTargetPull");
					AnyRadialForce->AccumulateMode = ERootMotionAccumulateMode::Additive;
					AnyRadialForce->Priority = 500;
					AnyRadialForce->Duration = Duration;
					AnyRadialForce->Radius = Radius;
					AnyRadialForce->Strength = Strength;
					AnyRadialForce->Location = CenterLocation;
					AnyRadialForce->bIsPush = false; // Pull
					AnyRadialForce->bNoZForce = true; // Typically keep them on ground/plane if it's a floor trap
					
					uint16 ID = MCC->ApplyRootMotionSource(AnyRadialForce);

					AppliedForces.Add({ MCC, ID });
					AffectedActors.Add(TargetChar);
					TargetsCount++;
				}
			}
		}
	}

	if (AffectedActors.Num() > 0)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnForceApplied.Broadcast(AffectedActors);
		}
	}

	if (Duration > 0.0f)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_ApplyMultiTargetPullForce::OnDurationFinished, Duration, false);
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_ApplyMultiTargetPullForce::OnDurationFinished()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinished.Broadcast();
	}
	EndTask();
}

void UAbilityTask_ApplyMultiTargetPullForce::OnDestroy(bool AbilityEnded)
{
	// Ensure we remove the sources if task is ended prematurely
	for (const auto& Info : AppliedForces)
	{
		if (Info.MovementComponent.IsValid())
		{
			Info.MovementComponent->RemoveRootMotionSourceByID(Info.RootMotionSourceID);
		}
	}
	AppliedForces.Empty();

	Super::OnDestroy(AbilityEnded);
}
