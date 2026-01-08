#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GravityTetherTargetActor.generated.h"

class USplineComponent;
class USplineMeshComponent;

/**
 * 
 */
UCLASS()
class BLASTERTPS_API AGravityTetherTargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	AGravityTetherTargetActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void CancelTargeting() override;
	virtual void Tick(float DeltaSeconds) override;

	// 弹道初始速度，应与Projectile保持一致
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Config")
	float ProjectileInitialSpeed = 2000.f;

	// 弹道重力缩放，应与Projectile保持一致
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Config")
	float ProjectileGravityScale = 1.0f;

	// 弹道半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Config")
	float ProjectileRadius = 10.f;

	// 用于可视化的样条线组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
	USplineComponent* SplineComponent;

	// 可视化网格（例如一个圆柱体或箭头）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	UStaticMesh* SplineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	UMaterialInterface* SplineMeshMaterial;

protected:
	// 缓存生成的样条线网格组件
	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshes;

	void UpdateSpline(const TArray<FVector>& PathPoints);
	FVector GetStartLocation() const;
};
