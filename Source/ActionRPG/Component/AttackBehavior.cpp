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

void UAttackBehavior::RegisterComboList(const TArray<FAttackData>& NewComboList)
{
	ComboList = NewComboList;
}

int32 UAttackBehavior::GetMaxCombo() const
{
	return ComboList.Num();
}


// Called when the game starts
void UAttackBehavior::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UAttackBehavior::Lock_Internal()
{
	CancelAttack();
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
		/*ComboIndex = 0;
		bCanStartNextAttack = true;*/
		ResetParameters();
	}

	//auto MovementComp = GetMovementComponent();
	//if (MovementComp.IsValid() && MovementComp->IsFalling())
	//{
	//	CancelAttack();
	//}
}

bool UAttackBehavior::ComboAttack()
{
	if (!ComboList.IsValidIndex(ComboIndex) || !bIsWithinComboWindow) {
		return false;
	}

	const FAttackData& Attack = ComboList[ComboIndex];
	if (RegisterAttackCommand(Attack))
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
	bIsWithinComboWindow = true;
	bCanStartNextAttack = true;

	GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
	ComboTimerHandle.Invalidate();
}

bool UAttackBehavior::RegisterAttackCommand(const FAttackData& Attack)
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

bool UAttackBehavior::IsAttacking() const
{
	bool bIsAttacking = CurrentAttack.IsValid() || PendingAttacks.Num() > 0;
	return bIsAttacking;
}

void UAttackBehavior::CancelAttack()
{
	auto AnimInstance = GetAnimInstance();
	if (CurrentAttack.IsValid() && AnimInstance.IsValid()) {
		AnimInstance->Montage_Stop(0.1f, CurrentAttack->AttackMontage);
	}
	PendingAttacks.Empty();
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
}

void UAttackBehavior::OnAnimNotifyAttackEnd()
{
	bCanStartNextAttack = true;
	GetMovementComponent()->SetJumpAllowed(true);
}

FAttackData UAttackBehavior::GetCurrentAttack() const
{
	static FAttackData DefaultsResult{};
	return CurrentAttack.IsValid() ? *CurrentAttack : DefaultsResult;
}
