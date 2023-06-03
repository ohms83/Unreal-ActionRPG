// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActorComponent.h"
#include "ActionRPG/Battle/BattleData.h"

#include "AttackBehavior.generated.h"

class UGameCharacterAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UAttackBehavior : public UGameActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackBehavior();

	UFUNCTION(BlueprintCallable)
	void RegisterComboList(const TArray<FAttackData>& NewComboList);

	/**
	* Try registering the next attack command from the combo list.
	* @return true if the combo command can be resitered; otherwise, false.
	*/
	UFUNCTION(BlueprintCallable)
	bool ComboAttack();

	/**
	* Try registering the next attack command. The attack will be stored in the command queue
	* and only be executed when ready.
	* @return true if the combo command can be resitered; otherwise, false.
	*/
	UFUNCTION(BlueprintCallable)
	bool RegisterAttackCommand(const FAttackData& NextAttack);

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

	UFUNCTION(BlueprintCallable)
	FAttackData GetCurrentAttack() const;

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
};
