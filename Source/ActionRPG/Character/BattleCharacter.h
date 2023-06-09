// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "CharacterData.h"

#include "ActionRPG/Battle/BattleData.h"
#include "ActionRPG/Actor/Equipment.h"

#include "BattleCharacter.generated.h"

class AEquipment;

DECLARE_MULTICAST_DELEGATE_OneParam(FGenericCharacterDelegate, class ABattleCharacter* /*ThisCharacter*/);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenericCharacterDynamicDelegate, class ABattleCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCharacterDeadDynamicDelegate, class ABattleCharacter*, DeadCharacter, class ABattleCharacter*, Attacker, struct FDamageEvent const&, DamageEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTakeMeleeDamageDynamicDelegate, class ABattleCharacter*, Attacker, struct FMeleeDamageEvent const& , DamageEvent, float, RealDamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttackHitDynamicDelegate, class ABattleCharacter*, Attacker, const FDamageEvent&, DamageEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterSpecialMoveDynamicDelegate, class ABattleCharacter*, Character, float, Countdown);

UENUM(BlueprintType)
enum class ECharacterOutlineType : uint8
{
	None			UMETA(DisplayName = "None"),
	Ally			UMETA(DisplayName = "Ally"),
	Enemy			UMETA(DisplayName = "Enemy"),
	Collectible		UMETA(DisplayName = "Collectible"),
};

UCLASS()
class ACTIONRPG_API ABattleCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABattleCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void OnJumped_Implementation() override;

	void ShowCharacterOutline(ECharacterOutlineType OutlineType);

private: // Stats
	UPROPERTY(EditAnywhere, Category = "Battle Character")
	FCharacterStats Stats;
	UPROPERTY(EditAnywhere, Category = "Battle Character")
	ECharacterTeam Team = ECharacterTeam::None;

public: // Stats
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	void UpdateStats();

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	const FCharacterStats& GetStats() const;
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	void SetStats(const FCharacterStats& NewStats, bool bUpdateStats = true);

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	void SetTeam(ECharacterTeam NewTeam);
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	ECharacterTeam GetTeam() const { return Team; }
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	bool IsSameTeam(AActor* const OtherActor) const;

	// TODO: Move to Battle Manager
	static float CalculateDamage(ABattleCharacter* Attacker, ABattleCharacter* Defender);

public: // Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	class UAttackBehavior* AttackBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	class UDodgeBehavior* DodgeBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	class UTargetSelectorComponent* TargetSelector = nullptr;
	/**
	* The outer capsule collision used for determining Parry and Flash Move.
	* If a block or dodge actions were trigger in the time between this collision was triggered
	* and before the actual inner collision was hit, the special Parry or Flash Move skills
	* will be activated.
	*/
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	class UCapsuleComponent* OuterCollision = nullptr;*/

public: // Attack
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	bool ExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackStart();
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackEnd();

	void OnWeaponHit(class AWeapon* Weapon, const TArray<FHitResult>& HitResults);
	void EnableTraceHit(bool bEnable);

public: // Attack
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Battle Character|Attack")
	FAttackHitDynamicDelegate OnAttackHitDelegate;

public: // Dodge
	bool ExecuteDodge(const FVector& Direction);

protected: // Dodge
	void OnDodgeFinished(ACharacter* DodgingActor);

public: // Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;
	
	void StartHitStop(float Seconds, float TimeDilation = 0.1f);
	void EndHitStop();

protected: // Damage
	void PlayDamageMontage(struct FDamageEvent const& DamageEvent, AActor* DamageCauser);
	void PlayHitFX(struct FDamageEvent const& DamageEvent, AActor* DamageCauser);
	void OnInvincibleFrameEnd();

public: // Damage
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Battle Character|Damage")
	FTakeMeleeDamageDynamicDelegate OnTakeMeleeDamageDelegate;

private: // Damage
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> FrontalDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> RearDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> LeftDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> RightDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	float InvincibleFrame = 0.3f;

	FTimerHandle InvincibleFrameHandle;
	FTimerHandle HitStopHandle;
	bool bCanTakeDamage = true;
	bool bHitStopEnabled = true;

public: // Knockback
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Damage")
	void Knockback(AActor* Attacker, const FVector2D& RelativeDirection, float Speed);
	// Compute a knockback direction in world-space
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Damage")
	FVector ConmputeKnockbackDirection(AActor* HitActor, const FVector2D& RelativeDirection);

protected: // Knockback
	// Periodically check whether the knockback has ended and reset bKnockback flag.
	void CheckKnockbackEnd();

private: // Knockback
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	bool bKnockback = false;

	FTimerHandle CheckKnockbackTimerHandle;

public: // Dead
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Dead")
	bool IsDead() const;

protected: // Dead
	void PlayDeadMontage();
	virtual void OnDead(FDamageEvent const& DamageEvent, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Character|Dead", meta = (DisplayName = "On Dead"))
	void OnDeadEvent_BP(FDamageEvent const& DamageEvent, AActor* DamageCauser);

public: // Dead
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Battle Character|Dead")
	FCharacterDeadDynamicDelegate OnCharacterDeadDynamicDelegate;

	FGenericCharacterDelegate OnDeadDelegate;

private: // Dead
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Dead", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeadMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Dead", meta = (AllowPrivateAccess = "true"))
	float DeadKnockbackSpeed = 600.f;

public: // Equipment
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Equipment")
	void Equip(AEquipment* Equipment, bool bUpdateStats = true);
	/**
	* Unequip the specified equipment. If the character isn't equipping that equipment
	* the function will silently return.
	* @param Equipment Equipment to be unequipped.
	* @param bDestroy Whether the unequipped weapon should also be destroy from the world.
	* @param bUpdateStats Whether the character's stats should also be updated. This is mainly
	*		 useful for removing batches of equipment and then manually calling UpdateStats at the end.
	*/
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Equipment")
	void Unequip(AEquipment* Equipment, bool bDestroy = true, bool bUpdateStats = true);
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Equipment")
	void UnequipAll(bool bDestroy = true, bool bUpdateStats = true);

protected: // Equipment
	// An internal function that contains the logics for equiping weapon
	void Equip_Weapon(AEquipment* Equipment);
	// An internal function that contains the logics for unequiping weapon
	void Unequip_Weapon(AEquipment* Equipment);

protected: // Equipment
	UPROPERTY()
	TMap<EEquipmentType, AEquipment*> EquipmentList;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Equipment")
	TMap<EEquipmentType, FName> EquipSockets;

	FDelegateHandle WeaponHitDelegate;

public: // Target
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Target")
	void SelectTarget(AActor* NextTarget);
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Target")
	AActor* GetSelectedTarget() const;
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Target")
	bool IsBeingTargetted() const;

public: // Special moves
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Special Moves")
	bool IsSpecialMoveFlag() const { return bSpecialMoveFlag; }
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Special Moves")
	float GetFlashMoveScaledCountdownTime() const;

protected:
	void TriggerFlashMove();
	void TriggerParry();
	void OnSpecialMoveEnd();

public:
	// TODO: Move out of character classes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Character|Debug")
	bool bAutoFlashMove = false;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Battle Character|Special Moves")
	FCharacterSpecialMoveDynamicDelegate OnFlashMoveDynamicDelegate;

private: // Special moves
	FTimerHandle SpecialMoveFrameHandle;
	// This flag will be raised when the Outer Collision was hit and before the inner
	// CollisionCylinder being hit. Any dodge or block actions triggered during this time
	// will activate Parry or Flash Move special moves.
	bool bSpecialMoveFlag = false;
	// A flag indicating whether this character can trigger Parry and Flash Move.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Character|Special Moves", meta = (AllowPrivateAccess = "true"))
	bool bCanTriggerSpecialMoves = true;
	// Special-move's effective time in seconds.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Character|Special Moves", meta = (AllowPrivateAccess = "true"))
	float SpecialMoveFrame = 3.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Character|Special Moves", meta = (AllowPrivateAccess = "true"))
	float FlashMoveGlobalTimeDilation = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Character|Special Moves", meta = (AllowPrivateAccess = "true"))
	float FlashMoveCharacterTimeDilation = 2.f;
};
