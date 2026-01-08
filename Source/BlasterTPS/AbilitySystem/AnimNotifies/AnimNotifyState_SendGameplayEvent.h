#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_SendGameplayEvent.generated.h"

/**
 * AnimNotifyState that can send GameplayEvents on Begin, End, and/or Tick.
 * Useful for defining hit windows or continuous effects.
 */
UCLASS()
class BLASTERTPS_API UAnimNotifyState_SendGameplayEvent : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	float EventMagnitude = 0.0f;

	// If true, the event is sent once when the notify state begins.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	bool bSendEventOnBegin = false;

	// If true, the event is sent once when the notify state ends.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	bool bSendEventOnEnd = false;

	// If true, the event is sent every tick while the notify state is active.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	bool bSendEventOnTick = true;
};

