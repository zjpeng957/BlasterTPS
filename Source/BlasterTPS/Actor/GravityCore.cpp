#include "GravityCore.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/EngineTypes.h"
#include "NiagaraComponent.h"

AGravityCore::AGravityCore()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(20.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GravityVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GravityVFXComp"));
	GravityVFXComponent->SetupAttachment(RootComponent);
	GravityVFXComponent->bAutoActivate = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f; // 启用重力产生抛物线
}

void AGravityCore::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AGravityCore::OnHit);
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGravityCore::OnOverlap);
	}
	
	SetLifeSpan(5.0f);
}

void AGravityCore::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetInstigator()) return;
	if (OtherActor == this) return;

	// Stop projectile to stick/float
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->ProjectileGravityScale = 0.f;
	}

	// Disable collision to prevent further hits/overlaps
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (MeshComponent)
	{
		MeshComponent->SetVisibility(false);
	}
	
	// Activate VFX
	if (GravityVFXComponent)
	{
		GravityVFXComponent->Activate();
	}

	// Start Gravity Effect
	// bIsGravityActive = true; // Logic moved to AbilityTask, but keeping variable logic simply for state if needed.

	// Apply damage if logic requires (e.g. direct hit damage)
	if (DamageEffectSpecHandle.IsValid())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	// Destroy after 8 seconds
	SetLifeSpan(8.0f);
	
	OnProjectileHit.Broadcast(Hit);
}

void AGravityCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGravityCore::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetInstigator())
	{
		return;
	}

	// 如果携带了 EffectSpec，应用给目标
	if (DamageEffectSpecHandle.IsValid())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	Destroy();
}
