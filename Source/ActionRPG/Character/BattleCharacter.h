// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "CharacterData.h"

#include "ActionRPG/Battle/BattleData.h"

#include "BattleCharacter.generated.h"

class UAttackBehavior;
class UDodgeBehavior;

UCLASS()
class ACTIONRPG_API ABattleCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABattleCharacter();

	// Try initiating the attack.
	void TryAttack();
	// TODO: A temporary variable just for testing
	UPROPERTY(EditAnywhere)
	FAttackData Attack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FCharacterStats Stats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component", meta = (AllowPrivateAccess = "true"))
	UAttackBehavior* AttackBehavior = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Component", meta = (AllowPrivateAccess = "true"))
	UDodgeBehavior* DodgeBehavior = nullptr;
};
