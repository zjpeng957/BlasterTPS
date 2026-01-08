#include "AnimNotifyState_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotifyState_SendGameplayEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (bSendEventOnBegin && MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData Payload;
			Payload.EventTag = EventTag;
			Payload.EventMagnitude = EventMagnitude;
			Payload.Instigator = OwnerActor;
			Payload.Target = OwnerActor;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
		}
	}
}

void UAnimNotifyState_SendGameplayEvent::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (bSendEventOnTick && MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData Payload;
			Payload.EventTag = EventTag;
			Payload.EventMagnitude = EventMagnitude;
			Payload.Instigator = OwnerActor;
			Payload.Target = OwnerActor;
			// You might want to pass DeltaTime in ContextHandle or related data if supported, but Magnitude works nicely for simple values.

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
		}
	}
}

void UAnimNotifyState_SendGameplayEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (bSendEventOnEnd && MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData Payload;
			Payload.EventTag = EventTag;
			Payload.EventMagnitude = EventMagnitude;
			Payload.Instigator = OwnerActor;
			Payload.Target = OwnerActor;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
		}
	}
}

