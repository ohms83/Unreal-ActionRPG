// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackBehavior.h"
#include "GameFramework/Character.h"

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
	const auto OwnerPawn = Cast<ACharacter>(GetOwner());
	if (IsValid(OwnerPawn))
	{
		const auto TempMesh = OwnerPawn->GetMesh();
		if (IsValid(TempMesh))
		{
			AnimInstance = Cast<UAnimInstance>(TempMesh->GetAnimInstance());
		}
	}
}


// Called every frame
void UAttackBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (PendingAttacks.Num() > 0 && bCanStartNextAttack)
	{
		PerformAttack(PendingAttacks[0]);
		PendingAttacks.RemoveAt(0);
	}

	if (CurrentAttackMontage.IsValid())
	{
		UAnimMontage* AttackMontage = CurrentAttackMontage.Get();
		if (!AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			CurrentAttackMontage.Reset();
			bCanStartNextAttack = true;
		}
	}
	else
	{
		bCanStartNextAttack = true;
	}
}

bool UAttackBehavior::RegisterAttack(const FAttackData& NextAttack)
{
	if (!bCanStartNextAttack) {
		return false;
	}
	PendingAttacks.Emplace(NextAttack);
	return true;
}

void UAttackBehavior::CancleAttack()
{
	if (!IsAttacking()) {
		return;
	}

	bCanStartNextAttack = true;
}

bool UAttackBehavior::PerformAttack(const FAttackData& NextAttack)
{
	CurrentAttackMontage = NextAttack.AttackMontage;
	UAnimMontage* AttackMontage = CurrentAttackMontage.Get();
	if (!IsValid(AttackMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid AttackMontage!"));
		return false;
	}

	// Play attack montage
	float Seconds = AnimInstance->Montage_Play(
		AttackMontage,
		NextAttack.AnimPlayRate,
		EMontagePlayReturnType::MontageLength,
		0);
	bCanStartNextAttack = false;
	return Seconds > 0;
}

void UAttackBehavior::OnAnimNotifyAttackStart()
{
	// TODO: Enable weapon's collision
}

void UAttackBehavior::OnAnimNotifyAttackEnd()
{
	// TODO: Disable weapon's collision
	bCanStartNextAttack = true;
}
