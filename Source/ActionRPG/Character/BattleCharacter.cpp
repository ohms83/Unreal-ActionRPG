// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "ActionRPG/Component/AttackBehavior.h"
#include "ActionRPG/Component/DodgeBehavior.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"

// Sets default values
ABattleCharacter::ABattleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackBehavior = CreateDefaultSubobject<UAttackBehavior>(TEXT("Attack Behavior"));
	DodgeBehavior = CreateDefaultSubobject<UDodgeBehavior>(TEXT("Dodge Behavior"));
}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABattleCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	UE_LOG(LogTemp, Log, TEXT("OnJumped_Implementation"));

	if (GetCharacterMovement()->CanAttemptJump() && AnimInstance.IsValid())
	{
		AttackBehavior->CancleAttack();
		AnimInstance->StopAllMontages(0.1f);
	}
}

void ABattleCharacter::OnAnimNotifyAttackStart()
{
	// TODO: Enable weapon's collision
	if (IsValid(AttackBehavior))
	{
		AttackBehavior->OnAnimNotifyAttackStart();
	}

	if (IsValid(DodgeBehavior))
	{
		DodgeBehavior->LockComponent(this);
	}
}

void ABattleCharacter::OnAnimNotifyAttackEnd()
{
	// TODO: Disable weapon's collision
	if (IsValid(AttackBehavior))
	{
		AttackBehavior->OnAnimNotifyAttackEnd();
	}

	if (IsValid(DodgeBehavior))
	{
		DodgeBehavior->UnlockComponent(this);
	}
}

void ABattleCharacter::ExecuteAttack()
{
	if (IsValid(AttackBehavior))
	{
		AttackBehavior->RegisterCombo();
	}
}

bool ABattleCharacter::ExecuteDodge(const FVector& Direction)
{
	if (IsValid(DodgeBehavior) && DodgeBehavior->Dodge(Direction))
	{
		AttackBehavior->CancleAttack();
		return true;
	}
	return false;
}
