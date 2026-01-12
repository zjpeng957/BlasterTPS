// #include "AbilityTask_WaitProjectileHit.h"
//
// UAbilityTask_WaitProjectileHit* UAbilityTask_WaitProjectileHit::WaitProjectileHit(UGameplayAbility* OwningAbility, AActor* Projectile)
// {
// 	UAbilityTask_WaitProjectileHit* MyTask = NewAbilityTask<UAbilityTask_WaitProjectileHit>(OwningAbility);
// 	MyTask->ProjectileActor = Projectile;
// 	return MyTask;
// }
//
// void UAbilityTask_WaitProjectileHit::Activate()
// {
// 	Super::Activate();
//
// 	if (ProjectileActor)
// 	{
// 		// Bind to the OnActorHit delegate
// 		// Note: This relies on the Actor dispatching hit events. 
// 		// Ensure the Projectile has bNotifyRigidBodyCollision or similar enabled if it's physics based, 
// 		// or that MovementComponent handles MoveBlocking and calls NotifyHit.
// 		ProjectileActor->OnActorHit.AddDynamic(this, &UAbilityTask_WaitProjectileHit::OnProjectileHit);
// 	}
// 	else
// 	{
// 		EndTask();
// 	}
// }
//
// void UAbilityTask_WaitProjectileHit::OnDestroy(bool AbilityEnded)
// {
// 	if (ProjectileActor)
// 	{
// 		ProjectileActor->OnActorHit.RemoveDynamic(this, &UAbilityTask_WaitProjectileHit::OnProjectileHit);
// 	}
//
// 	Super::OnDestroy(AbilityEnded);
// }
//
// void UAbilityTask_WaitProjectileHit::OnProjectileHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
// {
// 	if (ShouldBroadcastAbilityTaskDelegates())
// 	{
// 		OnHit.Broadcast(Hit);
// 	}
// 	
// 	// Usually one hit is enough for this task type
// 	EndTask();
// }
//
