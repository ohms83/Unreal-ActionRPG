// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "CharacterData.h"

#include "ActionRPG/Battle/BattleData.h"
#include "ActionRPG/Actor/Equipment.h"

#include "BattleCharacter.generated.h"

class UAttackBehavior;
class UDodgeBehavior;
class UTargetSelectorComponent;
class AEquipment;

DECLARE_MULTICAST_DELEGATE_OneParam(FGenericCharacterDelegate, class ABattleCharacter* /*ThisCharacter*/);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTakeMeleeDamageDelegate, class ABattleCharacter*, Attacker, struct FMeleeDamageEvent const& , DamageEvent, float, RealDamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttackHitDelegate, class ABattleCharacter*, Attacker, const FDamageEvent&, DamageEvent);

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

public: // Stats
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	void UpdateStats();

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	const FCharacterStats& GetStats() const;
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Stats")
	void SetStats(const FCharacterStats& NewStats, bool bUpdateStats = true);

	// TODO: Move to Battle Manager
	static float CalculateDamage(ABattleCharacter* Attacker, ABattleCharacter* Defender);

public: // Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	UAttackBehavior* AttackBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	UDodgeBehavior* DodgeBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component")
	UTargetSelectorComponent* TargetSelector = nullptr;

public: // Attack
	void ExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackStart();
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackEnd();

	void OnWeaponHit(class AWeapon* Weapon, const TArray<FHitResult>& HitResults);

public: // Attack
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Battle Character|Attack")
	FAttackHitDelegate OnAttackHitDelegate;

public: // Dodge
	bool ExecuteDodge(const FVector& Direction);

protected: // Dodge
	void OnDodgeFinished(ACharacter* DodgingActor);

public: // Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	
	void StartHitStop(float Seconds, float TimeDilation = 0.1f);
	void EndHitStop();

protected: // Damage
	void PlayDamageMontage(struct FDamageEvent const& DamageEvent, AActor* DamageCauser);
	void PlayHitFX(struct FDamageEvent const& DamageEvent, AActor* DamageCauser);
	void OnInvincibleFrameEnd();

public: // Damage
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Battle Character|Damage")
	FTakeMeleeDamageDelegate OnTakeMeleeDamageDelegate;

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

public:
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Damage")
	void Knockback(AActor* Attacker, const FVector2D& RelativeDirection, float Speed);
	// Compute a knockback direction in world-space
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Damage")
	FVector ConmputeKnockbackDirection(AActor* HitActor, const FVector2D& RelativeDirection);

protected:
	// Periodically check whether the knockback has ended and reset bKnockback flag.
	void CheckKnockbackEnd();

private:
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

public:
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

protected:
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

public:
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Target")
	void SelectTarget(AActor* NextTarget);
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Target")
	AActor* GetSelectedTarget() const;
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Target")
	bool IsBeingTargetted() const;
};
