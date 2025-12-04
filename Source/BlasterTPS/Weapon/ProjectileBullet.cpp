// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "BlasterTPS/BlasterComponents/LagCompensationComponent.h"
#include "BlasterTPS/Character/BlasterCharacter.h"
#include "BlasterTPS/PlayerController/BlasterPlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

#if  WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	if (ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner()))
	{
		if (ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->GetController()))
		{
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
			}
			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
			{
				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
					HitCharacter,
					TraceStart,
					InitialVelocity,
					OwnerController->GetServerTime() - OwnerController->SingleTripTime,
					nullptr
				);
			}
			UGameplayStatics::ApplyDamage(OtherActor, Damage, 
				OwnerController, this, UDamageType::StaticClass());
		}
	}
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	//FPredictProjectilePathParams PathParams;
	//PathParams.bTraceWithChannel = true;
	//PathParams.bTraceWithCollision = true;
	//PathParams.DrawDebugTime = 5.f;
	//PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	//PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	//PathParams.MaxSimTime = 4.f;
	//PathParams.ProjectileRadius = 5.f;
	//PathParams.SimFrequency = 30.f;
	//PathParams.StartLocation = GetActorLocation();
	//PathParams.TraceChannel = ECC_Visibility;
	//PathParams.ActorsToIgnore.Add(this);
	//FPredictProjectilePathResult PathResult;
	//UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
}
