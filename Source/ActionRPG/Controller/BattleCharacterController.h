// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonController.h"
#include "BattleCharacterController.generated.h"

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
	virtual void OnInputActionAttackSpecial();

	virtual void OnInputActionDodge();
	virtual void OnInputActionStopDodging();

	virtual void OnInputActionSelectNextTarget();

	UFUNCTION(BlueprintImplementableEvent)
	void InputActionDodge_Bluerprint();

	virtual void OnAnimationStateEnter(const FString& AnimStateName) override;

	UFUNCTION()
	void OnSpecialMoveTriggered(class ABattleCharacter* BattleCharacter, float Countdown);
	UFUNCTION()
	void OnSpecialMoveEnded(class ABattleCharacter* BattleCharacter, float Countdown);

	template<class T> T* GetPawnComponent() const
	{
		const APawn* aPawn = GetPawn();
		if (!IsValid(aPawn)) {
			return nullptr;
		}

		return Cast<T>(aPawn->GetComponentByClass(T::StaticClass()));
	}

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName AttackEventName = "Attack";
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName SpecialAttackEventName = "AttackSpecial";
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName DodgeEventName = "Dodge";
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName SelectNextTargetEventName = "SelectNextTarget";

	UPROPERTY()
	class UAttackBehavior* AttackBehavior = nullptr;
	UPROPERTY()
	class UDodgeBehavior* DodgeBehavior = nullptr;
	UPROPERTY()
	class UTargetSelectorComponent* TargetSelector = nullptr;

	bool bOneshotSpecialAttack = false;
};
