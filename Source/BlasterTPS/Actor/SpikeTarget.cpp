// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikeTarget.h"

#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbility.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BlasterTPS/Character/BlasterCharacter.h"

ASpikeTarget::ASpikeTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("ASpikeTarget Constructor called"));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ASpikeTarget::PostActorCreated()
{
	Super::PostActorCreated();
	UE_LOG(LogTemp, Warning, TEXT("ASpikeTarget::PostActorCreated"));
}

void ASpikeTarget::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UE_LOG(LogTemp, Warning, TEXT("ASpikeTarget::OnConstruction"));
}

void ASpikeTarget::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("spike target actor beginplay"));
}

void ASpikeTarget::StartTargeting(UGameplayAbility* Ability)
{
	UE_LOG(LogTemp, Warning, TEXT("ASpikeTarget::StartTargeting called"));
	Super::StartTargeting(Ability);

	if (Ability)
	{
		PrimaryPC = Ability->GetActorInfo().PlayerController.Get();
		// Add Targeting Tag to Source Actor's ASC
		if (UAbilitySystemComponent* ASC = Ability->GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(BlasterGameplayTags::State::Targeting);
		}

		// Add Targeting IMC
		if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(SourceActor))
		{
			BlasterChar->AddTargetingMappingContext();
		}
	}

	if (DecalClass && GetWorld())
	{
		SpawnedDecal = GetWorld()->SpawnActor<ADecalActor>(DecalClass);
		if (SpawnedDecal)
		{
			SpawnedDecal->SetActorHiddenInGame(true);
			if (UDecalComponent* DecalComp = SpawnedDecal->GetDecal())
			{
				DecalComp->DecalSize = FVector(DecalRadius);
				
				// 1. 如果指定了覆盖材质，应用它
				if (DecalMaterial)
				{
					DecalComp->SetDecalMaterial(DecalMaterial);
				}

				// 2. 立即创建动态材质，确保后续逻辑使用的是正确的材质实例
				DynamicMaterial = DecalComp->CreateDynamicMaterialInstance();

				// 3. 调试日志：确认当前使用的材质
				if (UMaterialInterface* CurrentMat = DecalComp->GetDecalMaterial())
				{
					UE_LOG(LogTemp, Warning, TEXT("SpikeTarget: Spawned Decal using material: %s"), *CurrentMat->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("SpikeTarget: Spawned Decal has NO material assigned!"));
				}
			}
		}
	}
}

void ASpikeTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SourceActor)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor))
		{
			ASC->RemoveLooseGameplayTag(BlasterGameplayTags::State::Targeting);
		}
		
		// Remove Targeting IMC
		if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(SourceActor))
		{
			BlasterChar->RemoveTargetingMappingContext();
		}
	}

	if (SpawnedDecal)
	{
		SpawnedDecal->Destroy();
	}
	Super::EndPlay(EndPlayReason);
}

void ASpikeTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static float LogTimer = 0.0f;
	LogTimer += DeltaSeconds;
	bool bShouldLog = LogTimer > 1.0f;
	if (bShouldLog) LogTimer = 0.0f;

	if (bShouldLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASpikeTarget::Tick running. SourceActor: %s"), *GetNameSafe(SourceActor));
	}

	if (PrimaryPC)
	{
		FVector ViewLoc;
		FRotator ViewRot;
		PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);

		StartLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		StartLocation.LiteralTransform = FTransform(ViewRot, ViewLoc);
	}
	
	// 1. 获取追踪结果
	// 基类 PerformTrace 会根据你在蓝图中设置的 MaxRange 和 TraceSphereRadius 执行射线/球体检测
	FHitResult HitResult = PerformTrace(SourceActor);

	if (bShouldLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("PerformTrace Hit: %s, Location: %s"), HitResult.bBlockingHit ? TEXT("True") : TEXT("False"), *HitResult.Location.ToString());
	}

	// 2. 验证命中是否有效
	if (HitResult.bBlockingHit)
	{
		if (SpawnedDecal)
		{
			SpawnedDecal->SetActorHiddenInGame(false);
			SpawnedDecal->SetActorLocation(HitResult.Location);
			
			// [修正] 让 Decal 的 X 轴（投影轴）指向法线的反方向（即指向地面内部）
			FVector ProjectionDir = -HitResult.ImpactNormal;
			FRotator TargetRot = ProjectionDir.Rotation();
			SpawnedDecal->SetActorRotation(TargetRot);
		}

		// 3. 距离判断 (使用基类的 MaxRange)
		FVector Origin = StartLocation.GetTargetingTransform().GetLocation();
		float Distance = FVector::Dist(Origin, HitResult.Location);
		bool bIsInRange = Distance <= MaxRange;

		if (SpawnedDecal && DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), bIsInRange ? ValidColor : InvalidColor);
		}
	}
	else
	{
		if (SpawnedDecal)
		{
			SpawnedDecal->SetActorHiddenInGame(true);
		}
	}
}
