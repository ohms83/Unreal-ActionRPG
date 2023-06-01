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

private: // Stats
	FCharacterStats Stats;

public: // Stats
	void UpdateStats();

private: // Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component", meta = (AllowPrivateAccess = "true"))
	UAttackBehavior* AttackBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component", meta = (AllowPrivateAccess = "true"))
	UDodgeBehavior* DodgeBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component", meta = (AllowPrivateAccess = "true"))
	UTargetSelectorComponent* TargetSelector = nullptr;

public: // Attack
	void ExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackStart();
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackEnd();

	void OnWeaponHit(class AWeapon* Weapon, const TArray<FHitResult>& HitResults);

public: // Dodge
	bool ExecuteDodge(const FVector& Direction);

protected:
	void OnDodgeFinished(ACharacter* DodgingActor);

public: // Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected: // Damage
	void PlayDamageMontage(struct FDamageEvent const& DamageEvent, AActor* DamageCauser);
	void PlayHitFX(struct FDamageEvent const& DamageEvent, AActor* DamageCauser);
	void OnInvincibleFrameEnd();

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
	bool bCanTkeDamage = true;

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
};
