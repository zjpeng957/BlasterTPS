#include "AnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
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

