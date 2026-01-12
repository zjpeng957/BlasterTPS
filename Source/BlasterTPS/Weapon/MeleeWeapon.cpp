#include "MeleeWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"

AMeleeWeapon::AMeleeWeapon()
{
	FireType = EFireType::EFT_HitScan;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AMeleeWeapon::PerformAttack(AActor* InstigatorActor, TSubclassOf<UGameplayEffect> DamageEffectClass, float DamageAmountOverride)
{
	if (!InstigatorActor) return;
	
	const FVector Start = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector End = Start + Forward * TraceLength;

	TArray<FHitResult> OutHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(InstigatorActor);

	// Perform sweep
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);

	if (bHit)
	{
		int32 Count = ActorsHitThisSwing.Num();
		if (Count >= MaxTargetsPerSwing) return;

		for (const FHitResult& Hit : OutHits)
		{
			AActor* Target = Hit.GetActor();
			if (!Target || ActorsHitThisSwing.Contains(Target) || Target == InstigatorActor) continue;

			ActorsHitThisSwing.Add(Target);
			Count++;

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			if (TargetASC && DamageEffectClass)
			{
				UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);
				if (SourceASC)
				{
					FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
					Context.AddSourceObject(this);
					
					FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);
					if (SpecHandle.IsValid())
					{
						float DamageToApply = (DamageAmountOverride > 0.f) ? DamageAmountOverride : GetDamage();
						SpecHandle.Data.Get()->SetSetByCallerMagnitude(BlasterGameplayTags::SetByCaller::Damage, DamageToApply);
						SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
				}
			}
			else
			{
				// Fallback to PointDamage if no ASC or no Effect class
				float DamageToApply = (DamageAmountOverride > 0.f) ? DamageAmountOverride : GetDamage();
				UGameplayStatics::ApplyPointDamage(Target, DamageToApply, Forward, Hit, InstigatorActor->GetInstigatorController(), this, nullptr);
			}

			if (Count >= MaxTargetsPerSwing) break;
		}
	}
}

void AMeleeWeapon::ResetHitActors()
{
	ActorsHitThisSwing.Empty();
}
