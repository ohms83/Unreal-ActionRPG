// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionRPG/Battle/BattleData.h"

#include "AttackBehavior.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UAttackBehavior : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackBehavior();

	UFUNCTION(BlueprintCallable)
	bool RegisterAttack(const FAttackData& NextAttack);

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const { return CurrentAttackMontage != nullptr; }

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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TWeakObjectPtr<UAnimMontage> CurrentAttackMontage;
	TArray<FAttackData> PendingAttacks;
	bool bCanStartNextAttack = true;

	UAnimInstance* AnimInstance = nullptr;
};
