// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BlasterTPS/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BlasterTPS/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/KismetMathLibrary.h>

#include "BlasterTPS/BlasterTPS.h"
#include "BlasterTPS/BlasterComponents/BuffComponent.h"
#include "BlasterTPS/BlasterComponents/LagCompensationComponent.h"
#include "BlasterTPS/GameMode/BlasterGameMode.h"
#include "BlasterTPS/PlayerController/BlasterPlayerController.h"
#include "BlasterTPS/PlayerState/BlasterPlayerState.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "BlasterTPS/Character/BlasterAttributeSet.h"
#include "BlasterTPS/AbilitySystem/Tags/BlasterGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);
	GetMesh()->SetCollisionObjectType(ECC_SKELETAL_MESH);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/**
	* Hit boxes for server-side rewind
	*/

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HITBOX);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HITBOX, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

bool ABlasterCharacter::IsLocallyReloading() const
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	UpdateHUDMana();
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BlasterPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("Added Input Mapping Context:%d %d"), IsLocallyControlled(), HasAuthority());
		}
	}
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}

void ABlasterCharacter::CalculateAOPitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch>90.f&&!IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}	
	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAOPitch();
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	bRotateRootBone = false;

	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

bool ABlasterCharacter::ActivateAbilityByTag(const FGameplayTag& AbilityTag) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
	}
	return false;
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();

	PollInit();

	//ABlasterPlayerController* CurController = Cast<ABlasterPlayerController>(GetController());
	//if (CurController)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("play tick:%p %s"), CurController, *(CurController->GetMatchState().ToString()));
	//}
	
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABlasterCharacter::StopJumping);
		EnhancedInputComponent->BindAction(EquipWeaponAction, ETriggerEvent::Started, this, &ABlasterCharacter::EquipButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABlasterCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Started, this, &ABlasterCharacter::ThrowGrenadeButtonPressed);
		
		// special abilities
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ABlasterCharacter::DashButtonPressed);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ABlasterCharacter::PrimaryButtonPressed);
		EnhancedInputComponent->BindAction(UltimateAction, ETriggerEvent::Started, this, &ABlasterCharacter::UltimateButtonPressed);

		// GAS Confirm/Cancel
		if (ConfirmAction)
		{
			EnhancedInputComponent->BindAction(ConfirmAction, ETriggerEvent::Triggered, GetAbilitySystemComponent(), &UAbilitySystemComponent::LocalInputConfirm);
		}
		if (CancelAction)
		{
			EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Triggered, GetAbilitySystemComponent(), &UAbilitySystemComponent::LocalInputCancel);
		}
	}
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector FowardDireation = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(FowardDireation, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ABlasterCharacter::Jump(const FInputActionValue& Value)
{
	if (Combat == nullptr) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		ACharacter::Jump();
	}
}

void ABlasterCharacter::StopJumping(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	ACharacter::StopJumping();
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeed(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		default:
			SectionName = FName("Rifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void ABlasterCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void ABlasterCharacter::OnRep_ReplicateMovement()
{
	Super::OnRep_ReplicateMovement();

	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::Elim()
{
	DropOrDestroyWeapons();
	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroyed();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}
}

void ABlasterCharacter::Destroyed()
{
	// Remove attribute delegates to avoid dangling references
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (HealthChangedDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
		}
		if (MaxHealthChangedDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedDelegateHandle);
		}
		if (ShieldChangedDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetShieldAttribute()).Remove(ShieldChangedDelegateHandle);
		}
		if (MaxShieldChangedDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMaxShieldAttribute()).Remove(MaxShieldChangedDelegateHandle);
		}
		if (MoveSpeedChangedDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMoveSpeedAttribute()).Remove(MoveSpeedChangedDelegateHandle);
		}
		if (JumpVelocityChangedDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetJumpVelocityAttribute()).Remove(JumpVelocityChangedDelegateHandle);
		}
	}

	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::MulticastElim_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastElim_%s: Role=%d RemoteRole=%d HasAuthority=%d"),
		*GetName(), (int32)GetLocalRole(), (int32)GetRemoteRole(), HasAuthority());
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();

	// Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(FName("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(FName("Glow"), 200.f);
	}
	StartDissolve();

	// Disable character movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	UE_LOG(LogTemp, Warning, TEXT("MulticastElim Disable Gameplay"));
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		const FVector CharacterLoc = GetActorLocation();
		FVector EliBotSpawnPoint(CharacterLoc.X, CharacterLoc.Y, CharacterLoc.Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			EliBotSpawnPoint,
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}
	if (
		IsLocallyControlled() &&
		Combat &&
		Combat->bAiming &&
		Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		ShowSniperScopeWidget(false);
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::EquipButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if(Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();
		if (
			Combat->ShouldSwapWeapons() && 
			!HasAuthority() && 
			Combat->CombatState == ECombatState::ECS_Unoccupied && 
			OverlappingWeapon == nullptr
			)
		{
			PlaySwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapon;
			bFinishSwaping = false;
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons())
		{
			Combat->SwapWeapons();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	//if (Combat && Combat->bHoldingTheFlag) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("blaster_crouch_button_pressed %s is trying to crouch"), *GetName());
		Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	//if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::ThrowGrenadeButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void ABlasterCharacter::DashButtonPressed(const FInputActionValue& Value)
{
	// Activate dash ability via ASC (GAS)
	if (GetAbilitySystemComponent())
	{
		// Try to activate by class. GA_Dash should be granted to the ASC (set in PlayerState DefaultAbilities)
		auto Result = ActivateAbilityByTag(BlasterGameplayTags::Abilities::Dash);
		UE_LOG(LogTemp, Warning, TEXT("try activate dash:%d"), Result);
	}
}

void ABlasterCharacter::PrimaryButtonPressed(const FInputActionValue& Value)
{
	if (GetAbilitySystemComponent())
	{
		ActivateAbilityByTag(BlasterGameplayTags::Abilities::Spike);
	}
}

void ABlasterCharacter::UltimateButtonPressed(const FInputActionValue& Value)
{
	if (GetAbilitySystemComponent())
	{
		if (GetAbilitySystemComponent()->HasMatchingGameplayTag(BlasterGameplayTags::State::Tethering))
		{
			// Send explode event
			FGameplayEventData EventData;
			EventData.EventTag = BlasterGameplayTags::Input::TetherSecPress;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, BlasterGameplayTags::Input::TetherSecPress, EventData);
		}
		else
		{
			ActivateAbilityByTag(BlasterGameplayTags::Abilities::GravityTether);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (bElimmed) return;

	UE_LOG(LogTemp, Warning, TEXT("ReceiveDamage_%s: Role=%d RemoteRole=%d HasAuthority=%d Damage=%.1f"),
		*GetName(), (int32)GetLocalRole(), (int32)GetRemoteRole(), HasAuthority(), Damage);
	// If we have an AbilitySystemComponent and GameplayEffects configured, prefer applying damage via ASC
	if (UAbilitySystemComponent* TargetASC = GetAbilitySystemComponent())
	{
		// Use DamageGameplayEffect which should be configured with BlasterDamageExecutionCalc
		if (DamageGameplayEffect)
		{
			UAbilitySystemComponent* SourceASC = nullptr;
			if (InstigatedBy)
			{
				SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatedBy->GetPawn());
				if (!SourceASC && InstigatedBy->PlayerState)
				{
					SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatedBy->PlayerState);
				}
			}
			if (!SourceASC && DamageCauser)
			{
				SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamageCauser);
				if (!SourceASC)
				{
					if (APawn* CauserPawn = Cast<APawn>(DamageCauser))
					{
						if (CauserPawn->GetPlayerState())
						{
							SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CauserPawn->GetPlayerState());
						}
					}
					else if (APawn* InstigatorPawn = DamageCauser->GetInstigator())
					{
						SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorPawn);
						if (!SourceASC && InstigatorPawn->GetPlayerState())
						{
							SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorPawn->GetPlayerState());
						}
					}
				}
			}

			UAbilitySystemComponent* ContextASC = SourceASC ? SourceASC : TargetASC;

			FGameplayEffectContextHandle EffectContext = ContextASC->MakeEffectContext();
			EffectContext.AddSourceObject(DamageCauser ? DamageCauser : this);
			EffectContext.AddInstigator(InstigatedBy, DamageCauser);

			FGameplayEffectSpecHandle SpecHandle = ContextASC->MakeOutgoingSpec(DamageGameplayEffect, 1.f, EffectContext);
			if (SpecHandle.IsValid())
			{
				// Pass the full damage amount. The ExecutionCalculation will handle splitting between Shield and Health.
				// We pass it as a negative value because we want to reduce attributes, but our logic in ExecutionCalc handles sign.
				// Let's pass it as negative to be consistent with "Damage" usually being a reduction if applied directly,
				// but here it's a magnitude.
				// In BlasterDamageExecutionCalc, we check if it's negative and flip it to positive for calculation, then apply negative modifiers.
				SpecHandle.Data->SetSetByCallerMagnitude(BlasterGameplayTags::SetByCaller::Damage, -Damage);
				ContextASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicateMovement();
		}
		CalculateAOPitch();
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw>90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw<-90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw)<15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

//void ABlasterCharacter::MulticastHit_Implementation()
//{
//	PlayHitReactMontage();
//}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	bool bIsCameraHide = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold;

	GetMesh()->SetVisibility(!bIsCameraHide);
	if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	{
		Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = bIsCameraHide;
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	return Velocity.Size();
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(GetHealth(), GetMaxHealth());
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(GetShield(), GetMaxShield());
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	UE_LOG(LogTemp, Warning, TEXT("start UpdateHUDAmmo"));
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateHUDAmmo:%d %d"), Combat->CarriedAmmo, Combat->EquippedWeapon->GetAmmo());
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void ABlasterCharacter::UpdateHUDMana()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDMana(GetMana(), GetMaxMana());
	}
}

void ABlasterCharacter::SpawnDefaultWeapon() const
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = false;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void ABlasterCharacter::ElimTimerFinished()
{
	if (ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>())
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(FName("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() const
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Initialize ability system on server when pawn is possessed
	InitializeAbilitySystem();

	// Ensure attributes are initialized now that ASC and PlayerState are available
	if (HasAuthority())
	{
		if (GetPlayerState<ABlasterPlayerState>())
		{
			if (DefaultAttributeEffect)
			{
				InitializeAttributes(DefaultAttributeEffect);
			}
		}
	}
	
	// After attributes are initialized, give default abilities on server
	GiveStartupAbilities();
}

// Add client-side PlayerState replication handler so clients can initialize their ASC
void ABlasterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Initialize Ability System Component on clients when PlayerState is replicatedOnRep_PlayerState
	InitializeAbilitySystem();

	// Cache the player state pointer for convenience
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;

	// Update cached attribute snapshots and HUD so client shows correct values immediately
	if (BlasterPlayerState)
	{
		// Ensure LastHealth/LastShield are in sync
		LastHealth = BlasterPlayerState->GetHealth();
		LastShield = BlasterPlayerState->GetShield();

		// Update HUD elements
		UpdateHUDHealth();
		UpdateHUDShield();
		UpdateHUDAmmo();
		UpdateHUDMana();
	}
	UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState called for %s"), *GetName());
}

void ABlasterCharacter::InitializeAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializeAbilitySystem: AbilitySystemComponent already exists for %s"), *GetName());
		return; // ensure not re-init
	}

	if (ABlasterPlayerState* BPS = GetPlayerState<ABlasterPlayerState>())
	{
		if (UAbilitySystemComponent*  ASC = BPS->GetAbilitySystemComponent())
		{
			// Initialize actor info linking ASC to avatar/owner
			ASC->InitAbilityActorInfo(BPS, this);
			// Cache pointer locally
			AbilitySystemComponent = ASC;
			UE_LOG(LogTemp, Warning, TEXT("InitializeAbilitySystem: ASC found and cached for %s. ASC: %s"), *GetName(), *ASC->GetName());

			// Subscribe to attribute change delegates on ASC
			HealthChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetHealthAttribute()).AddUObject(this, &ABlasterCharacter::OnHealthChanged);
			MaxHealthChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ABlasterCharacter::OnMaxHealthChanged);
			ShieldChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetShieldAttribute()).AddUObject(this, &ABlasterCharacter::OnShieldChanged);
			MaxShieldChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMaxShieldAttribute()).AddUObject(this, &ABlasterCharacter::OnMaxShieldChanged);
			MoveSpeedChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ABlasterCharacter::OnMoveSpeedChanged);
			JumpVelocityChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetJumpVelocityAttribute()).AddUObject(this, &ABlasterCharacter::OnJumpVelocityChanged);
			ManaChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetManaAttribute()).AddUObject(this, &ABlasterCharacter::OnManaChanged);
			MaxManaChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UBlasterAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ABlasterCharacter::OnMaxManaChanged);

			// Initialize cached values
			LastHealth = GetHealth();
			LastShield = GetShield();
			
			UE_LOG(LogTemp, Warning, TEXT("InitializeAbilitySystem success for %s. LastShield: %.1f"), *GetName(), LastShield);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InitializeAbilitySystem: ASC is null in PlayerState for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeAbilitySystem: PlayerState is null for %s"), *GetName());
	}
}

void ABlasterCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	float NewMoveSpeed = Data.NewValue;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewMoveSpeed;
		UpdateHUDAmmo(); // not directly related, but keep HUD update pattern; consider creating UpdateHUDMovement
	}
}

void ABlasterCharacter::OnJumpVelocityChanged(const FOnAttributeChangeData& Data)
{
	float NewJumpVelocity = Data.NewValue;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->JumpZVelocity = NewJumpVelocity;
	}
}


void ABlasterCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	// Update HUD
	UpdateHUDHealth();
	// Play hit react if decreased
	if (NewHealth < LastHealth)
	{
		ActivateAbilityByTag(BlasterGameplayTags::Abilities::HitReact);
	}
	LastHealth = NewHealth;
}

void ABlasterCharacter::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdateHUDHealth();
}

void ABlasterCharacter::OnShieldChanged(const FOnAttributeChangeData& Data)
{
	float NewShield = Data.NewValue;
	UE_LOG(LogTemp, Warning, TEXT("OnShieldChanged for %s. NewShield: %.1f, LastShield: %.1f"), *GetName(), NewShield, LastShield);
	UpdateHUDShield();
	if (NewShield < LastShield)
	{
		ActivateAbilityByTag(BlasterGameplayTags::Abilities::HitReact);
	}
	LastShield = NewShield;
}

void ABlasterCharacter::OnMaxShieldChanged(const FOnAttributeChangeData& Data)
{
	UpdateHUDShield();
}

void ABlasterCharacter::OnManaChanged(const FOnAttributeChangeData& Data)
{
	UpdateHUDMana();
}

void ABlasterCharacter::OnMaxManaChanged(const FOnAttributeChangeData& Data)
{
	UpdateHUDMana();
}

void ABlasterCharacter::AddTargetingMappingContext()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (TargetingMappingContext)
			{
				Subsystem->AddMappingContext(TargetingMappingContext, TargetingMappingPriority);
			}
		}
	}
}

void ABlasterCharacter::RemoveTargetingMappingContext()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (TargetingMappingContext)
			{
				Subsystem->RemoveMappingContext(TargetingMappingContext);
			}
		}
	}
}











