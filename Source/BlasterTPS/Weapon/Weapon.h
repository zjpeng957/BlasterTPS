// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterTPS/HUD/BlasterHUD.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTERTPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	/*
 * Textures for the weapon crosshairs
 */

	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere)
	bool bAutomatic = true;

	UFUNCTION()
	void OnRep_WeaponState();

	void ShowPickupWidget(bool bShowWidget);

	void SetWeaponState(EWeaponState State);

	virtual void Fire(const FVector& HitTarget);

	FORCEINLINE class USphereComponent* GetAreaSphere() const { return AreaCollision; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomFOV() const { return ZoomFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaCollision;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
};
