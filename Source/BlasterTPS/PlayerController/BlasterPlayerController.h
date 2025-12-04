// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterTPS/GameMode/BlasterGameMode.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
/**
 * 
 */
UCLASS()
class BLASTERTPS_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	FORCEINLINE FName GetMatchState() const { return MatchState; }

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);

	float SingleTripTime = 0;
	FHighPingDelegate HighPingDelegate;
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	/*
	 * Sync time between server and client
	 */
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category=Time)
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTIme = 0.f;

	void CheckTimeSync(float DeltaTime);
	void PollInit();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	float MatchTime = 30.f;
	float WarmupTime = 10.f;
	float LevelStartingTime = 0.f;
	float CooldownTime = 10.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	bool bInitializedHealth = false;
	float HUDShield;
	float HUDMaxShield;
	bool bInitializedShield = false;
	float HUDScore;
	bool bInitializedScore = false;
	int32 HUDDefeats;
	bool bInitializedDefeats = false;
	int32 HUDGrenades;
	bool bInitializedGrenades = false;
	float HUDCarriedAmmo;
	bool bInitializedCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool bInitializedWeaponAmmo = false;

	float HighPingRunningTime = 0.f;
	float PingAnimationRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);
};
