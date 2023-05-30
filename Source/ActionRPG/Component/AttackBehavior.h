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
