#include "Ability_Dash.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"
#include "BlasterTPS/Character/BlasterCharacterBase.h"
#include  "GameplayAbilities/Public/Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include  "GameplayAbilities/Public/Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UAbility_Dash::UAbility_Dash()
{
	
}

void UAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("activate ability dash"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	// 1. 施加无敌 GE
	for (auto EffectClass: DashEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass);
			
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
			}
		}
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, 
		NAME_None, 
		DashMontage, 
		1.0f, 
		NAME_None, 
		true // 即使被中断也停止
	);
	MontageTask->ReadyForActivation();

	// 2. 计算冲刺方向
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ABlasterCharacterBase* Char = Cast<ABlasterCharacterBase>(GetAvatarActorFromActorInfo());
	FVector DashDirection = Char->GetLastMovementInputVector(); // 可能是局部空间
	// 3. 创建 ApplyRootMotionConstantForce 任务
	// 参数说明：Task名称, 强力方向, 强度, 持续时间, 是否施加重力...
	UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this, 
		FName("DashTask"), 
		DashDirection, 
		DashDistance / DashDuration, // 强度即速度
		DashDuration, 
		false,      // IsStrengthDistanceDependent
		nullptr,    // StrengthOverTimeCurve
		ERootMotionFinishVelocityMode::SetVelocity, // 结束后的速度处理
		FVector::ZeroVector, 
		0.f, 
		false       // EnableGravity
	);

	// 4. 绑定结束回调
	RootMotionTask->OnFinish.AddDynamic(this, &UAbility_Dash::OnDashFinished);
	RootMotionTask->ReadyForActivation();
	ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Dash")));
} 

void UAbility_Dash::OnDashFinished()
{ 
	// 5. 冲刺结束后的清理逻辑（如移除 Tag 或 结束技能）
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		for (auto EffectClass: DashEffects)
		{
			if (EffectClass)
			{
				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass);
			
				if (SpecHandle.IsValid())
				{
					// 移除激活的effect
				}
			}
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}