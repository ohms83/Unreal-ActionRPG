// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackBehavior.h"

#include "TimerManager.h"
#include "GameFramework/Character.h"

#include "ActionRPG/Character/GameCharacterAnimInstance.h"

// Sets default values for this component's properties
UAttackBehavior::UAttackBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackBehavior::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UAttackBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (PendingAttacks.Num() > 0 && bCanStartNextAttack)
	{
		CurrentAttack = PendingAttacks[0];

		if (CurrentAttack.IsValid()) {
			PerformAttack(*CurrentAttack.Get());
		}

		PendingAttacks.RemoveAt(0);
	}

	if (CurrentAttack.IsValid())
	{
		const UAnimMontage* AttackMontage = CurrentAttack->AttackMontage;
		auto AnimInstance = GetAnimInstance();
		if (AnimInstance.IsValid() && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			ResetParameters();
		}
	}
	else
	{
		bCanStartNextAttack = true;
	}

	//auto MovementComp = GetMovementComponent();
	//if (MovementComp.IsValid() && MovementComp->IsFalling())
	//{
	//	CancleAttack();
	//}
}

bool UAttackBehavior::RegisterCombo()
{
	if (!ComboList.IsValidIndex(ComboIndex) || !bIsWithinComboWindow) {
		return false;
	}

	const FAttackData& Attack = ComboList[ComboIndex];
	if (RegisterAttack(Attack))
	{
		if (++ComboIndex >= ComboList.Num())
		{
			bIsWithinComboWindow = false;
			ComboTimerHandle.Invalidate();
		}
		else
		{
			StartComboCountdown();
		}

		return true;
	}
	
	return false;
}

void UAttackBehavior::StartComboCountdown()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		ComboTimerHandle,
		this,
		&UAttackBehavior::OnComboCountdownEnded,
		ComboWindow,
		false
	);

	bIsWithinComboWindow = true;
}

void UAttackBehavior::OnComboCountdownEnded()
{
	// Disable combo attack until the current attack animation is finished.
	ComboIndex = ComboList.Num();
	bIsWithinComboWindow = false;
}

void UAttackBehavior::ResetParameters()
{
	CurrentAttack.Reset();
	ComboIndex = 0;
	ComboTimerHandle.Invalidate();
	bIsWithinComboWindow = true;
	bCanStartNextAttack = true;
}

bool UAttackBehavior::RegisterAttack(const FAttackData& Attack)
{
	if (IsLocked())
	{
		return false;
	}

	//auto MovementComp = GetMovementComponent();
	//if (MovementComp.IsValid() && MovementComp->IsFalling())
	//{
	//	return false;
	//}

	TSharedPtr<FAttackData> NextAttack = MakeShared<FAttackData>(Attack);
	PendingAttacks.Emplace(NextAttack);
	return true;
}

void UAttackBehavior::CancleAttack()
{
	// TODO: Stop playing montage
	auto AnimInstance = GetAnimInstance();
	if (CurrentAttack.IsValid() && AnimInstance.IsValid()) {
		AnimInstance->Montage_Stop(0.1f, CurrentAttack->AttackMontage);
	}
	ResetParameters();
}

bool UAttackBehavior::PerformAttack(const FAttackData& NextAttack)
{
	if (!IsValid(NextAttack.AttackMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid AttackMontage!"));
		return false;
	}

	// Play attack montage
	auto AnimInstance = GetAnimInstance();
	bool bIsMontagePlayed = AnimInstance->Montage_Play(
		NextAttack.AttackMontage,
		NextAttack.AnimPlayRate,
		EMontagePlayReturnType::MontageLength,
		0) > 0;

	if (bIsMontagePlayed)
	{
		bCanStartNextAttack = false;
		GetMovementComponent()->SetJumpAllowed(false);
	}

	return bIsMontagePlayed;
}

void UAttackBehavior::OnAnimNotifyAttackStart()
{
	UE_LOG(LogTemp, Log, TEXT("%s can jump %s"),
		*GetOwner()->GetName(),
		GetMovementComponent()->CanAttemptJump() ? TEXT("TRUE") : TEXT("FALSE"));
}

void UAttackBehavior::OnAnimNotifyAttackEnd()
{
	bCanStartNextAttack = true;
	GetMovementComponent()->SetJumpAllowed(true);
	UE_LOG(LogTemp, Log, TEXT("UAttackBehavior::OnAnimNotifyAttackEnd()"));
}
