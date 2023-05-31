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

public: // Attack
	void ExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackStart();
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Attack")
	void OnAnimNotifyAttackEnd();

public: // Dodge
	bool ExecuteDodge(const FVector& Direction);

public: // Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected: // Damage
	void PlayDamageMontage(struct FDamageEvent const& DamageEvent);

private: // Damage
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> FrontalDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> RearDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> LeftDamageMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Damage", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> RightDamageMontages;

public: // Equipment
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Equipment")
	void Equip(AEquipment* Equipment, bool bUpdateStats = true);
	UFUNCTION(BlueprintCallable, Category = "Battle Character|Equipment")
	void Unequip(AEquipment* Equipment, bool bUpdateStats = true);

protected: // Equipment
	UPROPERTY()
	TMap<EEquipmentType, AEquipment*> EquipmentList;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Equipment")
	TMap<EEquipmentType, FName> EquipSockets;
};
