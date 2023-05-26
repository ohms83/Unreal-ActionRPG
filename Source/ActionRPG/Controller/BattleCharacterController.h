// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonController.h"
#include "BattleCharacterController.generated.h"

class UAttackBehavior;
class UDodgeBehavior;

/**
 * 
 */
UCLASS()
class ACTIONRPG_API ABattleCharacterController : public AThirdPersonController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void OnInputActionAttack();

	virtual void OnInputActionDodge();
	virtual void OnInputActionStopDodging();

	UFUNCTION(BlueprintImplementableEvent)
	void InputActionDodge_Bluerprint();

	virtual void OnAnimationStateEnter(const FString& AnimStateName) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName AttackEventName = "Attack";
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName DodgeEventName = "Dodge";

	UAttackBehavior* AttackBehavior = nullptr;
	UDodgeBehavior* DodgeBehavior = nullptr;
};
