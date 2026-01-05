// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "BlasterTPS/BlasterComponents/BuffComponent.h"
#include "BlasterTPS/BlasterComponents/CombatComponent.h"
#include "BlasterTPS/BlasterTypes/CombatState.h"
#include "BlasterTPS/BlasterTypes/TurningInPlace.h"
#include "BlasterTPS/PlayerState/BlasterPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "BlasterCharacter.generated.h"

class ULagCompensationComponent;
class UBoxComponent;
class ABlasterPlayerState;

UCLASS()
class BLASTERTPS_API ABlasterCharacter : public ABlasterCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	// Ability System initialization
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	void InitializeAbilitySystem();

	// Attribute change handlers
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnShieldChanged(const FOnAttributeChangeData& Data);
	void OnMaxShieldChanged(const FOnAttributeChangeData& Data);
	void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);
	void OnJumpVelocityChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnMaxManaChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* EquipWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ThrowGrenadeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* UltimateAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ConfirmAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CancelAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* TargetingMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 TargetingMappingPriority = 1;

	void AddTargetingMappingContext();
	void RemoveTargetingMappingContext();

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	void StopJumping(const FInputActionValue& Value);

	void EquipButtonPressed(const FInputActionValue& Value);

	void CrouchButtonPressed(const FInputActionValue& Value);

	void AimButtonPressed(const FInputActionValue& Value);

	void AimButtonReleased(const FInputActionValue& Value);

	void FireButtonPressed(const FInputActionValue& Value);

	void FireButtonReleased(const FInputActionValue& Value);

	void ReloadButtonPressed(const FInputActionValue& Value);

	void ThrowGrenadeButtonPressed(const FInputActionValue& Value);

	// Activate dash ability (GAS)
	void DashButtonPressed(const FInputActionValue& Value);

	void PrimaryButtonPressed(const FInputActionValue& Value);

	void UltimateButtonPressed(const FInputActionValue& Value);
	
	bool IsWeaponEquipped();

	bool IsAiming();

	void SetOverlappingWeapon(AWeapon* Weapon);
	AWeapon* GetEquippedWeapon() const;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void PollInit();
	void RotateInPlace(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void UpdateHUDMana();
	void SpawnDefaultWeapon() const;

	FORCEINLINE float GetAOYaw() const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { if (const ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) return PS->GetHealth(); return 0.f; }
	FORCEINLINE void SetHealth(float Amount) { if (ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) PS->SetHealth(Amount); }
	FORCEINLINE float GetMaxHealth() const { if (const ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) return PS->GetMaxHealth(); return 0.f; }
	FORCEINLINE float GetShield() const { if (const ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) return PS->GetShield(); return 0.f; }
	FORCEINLINE void SetShield(float Amount) { if (ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) PS->SetShield(Amount); }
	FORCEINLINE float GetMaxShield() const { if (const ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) return PS->GetMaxShield(); return 0.f; }
	FORCEINLINE float GetMana() const { if (const ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) return PS->GetMana(); return 0.f; }
	FORCEINLINE void SetMana(float Amount) { if (ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) PS->SetMana(Amount); }
	FORCEINLINE float GetMaxMana() const { if (const ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>()) return PS->GetMaxMana(); return 0.f; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage()const { return ReloadMontage; }
	ECombatState GetCombatState() const;
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade()const { return AttachedGrenade; }
	bool IsLocallyReloading() const;

	/**
	* Hit boxes used for server-side rewind
	*/
	UPROPERTY(EditAnywhere)
	UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

	UPROPERTY()
	TMap<FName, UBoxComponent*> HitCollisionBoxes;

	bool bFinishSwaping = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CalculateAOPitch();

	void AimOffset(float DeltaTime);

	void SimProxiesTurn();

	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();
	
	// Cached last values for reacting to changes
	float LastHealth = 0.f;
	float LastShield = 0.f;

	// Delegate handles so we can remove bindings on destroy
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ShieldChangedDelegateHandle;
	FDelegateHandle MaxShieldChangedDelegateHandle;
	FDelegateHandle MoveSpeedChangedDelegateHandle;
	FDelegateHandle JumpVelocityChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;

	bool ActivateAbilityByTag(const FGameplayTag& AbilityTag) const;

public: 	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	void PlayFireMontage(bool bAiming);

	void PlayHitReactMontage();

	void PlayElimMontage();

	void PlayReloadMontage();
	
	void PlayThrowGrenadeMontage();

	void PlaySwapMontage();

	virtual void OnRep_ReplicateMovement() override;

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	virtual void Destroyed() override;

private:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/*
	 * Blaster components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
	ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw; 
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	void TurnInPlace(float DeltaTime);

	/*
	 * Animation Montages
	 */
	UPROPERTY(EditAnywhere, Category=Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 20.f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

	float CalculateSpeed();

	/*
	 * Player Shield
	 * (Moved to AttributeSet in BlasterCharacterBase)
	 */
	// Shield is now part of AttributeSet; remove local replicated fields and OnRep.

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	/*
	 * Dissolve effect
	 */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(VisibleAnywhere, Category=Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;

	/*
	 * Elim bot
	 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
	/**
	* Grenade
	*/
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// GameplayEffect to apply when dealing health damage (optional, set in editor)
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	// GameplayEffect to apply when dealing shield damage (optional, set in editor)
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> ShieldGameplayEffect;
};
