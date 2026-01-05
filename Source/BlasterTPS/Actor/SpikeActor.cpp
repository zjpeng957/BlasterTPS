#include "SpikeActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"

ASpikeActor::ASpikeActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	OverlapComp->SetupAttachment(RootComponent);
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InitialLifeSpan = 30.0f;
}

void ASpikeActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ASpikeActor::OnOverlapBegin);
		OverlapComp->OnComponentEndOverlap.AddDynamic(this, &ASpikeActor::OnOverlapEnd);
	}
}

void ASpikeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner()) return; // Don't damage owner (PlayerState)
	if (OtherActor == GetInstigator()) return; // Don't damage instigator (Character)

	if (DamageEffectClass)
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			ContextHandle.AddInstigator(GetInstigator(), this);
			
			// Apply periodic damage effect
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				ActiveEffectHandles.Add(OtherActor, ActiveHandle);
			}
		}
	}
}

void ASpikeActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ActiveEffectHandles.Contains(OtherActor))
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandles[OtherActor]);
		}
		ActiveEffectHandles.Remove(OtherActor);
	}
}
