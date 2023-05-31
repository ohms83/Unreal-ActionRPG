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

void ABattleCharacter::UpdateStats()
{
	for (const auto& KeyValue : EquipmentList)
	{
		const auto& Equipment = KeyValue.Value;
		Stats += Equipment->GetStats();
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

	AEquipment** WeaponPtr = EquipmentList.Find(EEquipmentType::Weapon);
	if (WeaponPtr)
	{
		AWeapon* Weapon = Cast<AWeapon>(*WeaponPtr);
		if (IsValid(Weapon))
		{
			Weapon->EnableTraceHit(true);
		}
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

	AEquipment** WeaponPtr = EquipmentList.Find(EEquipmentType::Weapon);
	if (WeaponPtr)
	{
		AWeapon* Weapon = Cast<AWeapon>(*WeaponPtr);
		if (IsValid(Weapon))
		{
			Weapon->EnableTraceHit(false);
		}
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

float ABattleCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float RealDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (RealDamageAmount <= 0) {
		return 0;
	}

	PlayDamageMontage(DamageEvent);
	return RealDamageAmount;
}

void ABattleCharacter::PlayDamageMontage(FDamageEvent const& DamageEvent)
{
	if (FrontalDamageMontages.Num() <= 0) {
		return;
	}

	int32 Index = FMath::RandRange(0, (int32)FrontalDamageMontages.Num() - 1);
	UAnimMontage* DamageMontage = FrontalDamageMontages[Index];
	AnimInstance->Montage_Play(DamageMontage, 1.0f);
}

void ABattleCharacter::Equip(AEquipment* Equipment, bool bUpdateStats)
{
	if (!IsValid(Equipment) || Equipment->GetEquipType() == EEquipmentType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid equipment!"));
		return;
	}

	const EEquipmentType EquipType = Equipment->GetEquipType();
	AEquipment* CurrentEquipment = EquipmentList.FindOrAdd(EquipType);
	if (IsValid(CurrentEquipment)) {
		Unequip(CurrentEquipment, false);
	}

	FAttachmentTransformRules TransformRules = {
		// InLocationRule
		EAttachmentRule::KeepRelative,
		// InRotationRule
		EAttachmentRule::KeepRelative,
		// InScaleRule
		EAttachmentRule::KeepRelative,
		// bWeldSimulatedBodies
		false
	};

	const FName* SocketName = EquipSockets.Find(EquipType);
	if (SocketName) {
		Equipment->AttachToComponent(GetMesh(), TransformRules, *SocketName);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Invalid equipment socket of type=%d"), (int32)EquipType);
	}

	EquipmentList.Add(EquipType, Equipment);

	if (bUpdateStats) {
		UpdateStats();
	}
}

void ABattleCharacter::Unequip(AEquipment* Equipment, bool bUpdateStats)
{
	if (!IsValid(Equipment) || Equipment->GetEquipType() == EEquipmentType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid equipment!"));
		return;
	}

	const EEquipmentType EquipType = Equipment->GetEquipType();
	AEquipment* CurrentEquipment = EquipmentList.FindOrAdd(EquipType);
	if (IsValid(CurrentEquipment) && CurrentEquipment == Equipment) {
		EquipmentList.Remove(EquipType);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Cannot unequip an invalid equipment!"));
	}

	if (bUpdateStats) {
		UpdateStats();
	}
}
