// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionRPG/Battle/BattleData.h"

#include "AttackBehavior.generated.h"

class UGameCharacterAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UAttackBehavior : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackBehavior();

	UFUNCTION(BlueprintCallable)
	bool RegisterCombo();

	UFUNCTION(BlueprintCallable)
	bool RegisterAttack(const FAttackData& NextAttack);

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const { return CurrentAttack.IsValid(); }

	// Cancle the current attack.
	UFUNCTION(BlueprintCallable)
	void CancleAttack();

	UFUNCTION(BlueprintCallable)
	void SetCanStartNextAttack(bool bFlag) { bCanStartNextAttack = bFlag; }

	// An event handler that will be triggered from the Animation Blueprint when received an AnimNotify_AttackStart
	UFUNCTION(BlueprintCallable)
	void OnAnimNotifyAttackStart();
	// An event handler that will be triggered from the Animation Blueprint when received an AnimNotify_AttackEnd
	UFUNCTION(BlueprintCallable)
	void OnAnimNotifyAttackEnd();

	/**
	* Lock this component from executing any commands. If the component is already locked by
	* the other object, the function return false.
	* @param Locker A pointer to the object that's calling this function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool LockComponent(const UObject* Locker);

	/**
	* Unlock this component from executing any commands. If the component wasn't locked by
	* the given Locker, the function silently returns.
	* @param Locker A pointer to the object that's calling this function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	void UnlockComponent(const UObject* Locker);

	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool IsLocked() const { return ComponentLocker.IsValid(); }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool PerformAttack(const FAttackData& NextAttack);

	void StartComboCountdown();
	void OnComboCountdownEnded();

private:
	void ResetParameters();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FAttackData> ComboList;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float ComboWindow = 0.3f;

	FTimerHandle ComboTimerHandle;
	int32 ComboIndex = 0;
	bool bIsWithinComboWindow = true;

	TSharedPtr<FAttackData> CurrentAttack;
	TArray<TSharedPtr<FAttackData>> PendingAttacks;
	bool bCanStartNextAttack = true;

	// An object that acquire a lock to this component.
	TWeakObjectPtr<const UObject> ComponentLocker;

	TWeakObjectPtr<UGameCharacterAnimInstance> AnimInstance;
};
