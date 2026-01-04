// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "SpikeTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERTPS_API ASpikeTarget : public AGameplayAbilityTargetActor_GroundTrace
{
	GENERATED_BODY()
	
public:
	ASpikeTarget();
	virtual void Tick(float DeltaSeconds) override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void PostActorCreated() override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Visualization")
	TSubclassOf<class ADecalActor> DecalClass;

	// [新增] 强制指定贴花材质（填入你的圆形材质）
	UPROPERTY(EditAnywhere, Category = "Visualization")
	UMaterialInterface* DecalMaterial;

	UPROPERTY()
	class ADecalActor* SpawnedDecal;

	UPROPERTY(EditAnywhere)
	float DecalRadius = 300.f;

	// 在构造函数或 BeginPlay 中创建的材质实例
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	// 无法放置时的颜色
	UPROPERTY(EditAnywhere, Category = "Settings")
	FLinearColor InvalidColor = FLinearColor::Red;

	// 可以放置时的颜色
	UPROPERTY(EditAnywhere, Category = "Settings")
	FLinearColor ValidColor = FLinearColor::Blue;
};
