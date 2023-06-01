// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "ActionRPG/Component/AttackBehavior.h"
#include "ActionRPG/Component/DodgeBehavior.h"
#include "ActionRPG/Component/TargetSelectorComponent.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ABattleCharacter::ABattleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackBehavior = CreateDefaultSubobject<UAttackBehavior>(TEXT("Attack Behavior"));
	DodgeBehavior = CreateDefaultSubobject<UDodgeBehavior>(TEXT("Dodge Behavior"));
	TargetSelector = CreateDefaultSubobject<UTargetSelectorComponent>(TEXT("Target Selector"));
}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();

	DodgeBehavior->OnDodgeFinished.AddUObject(this, &ABattleCharacter::OnDodgeFinished);
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

void ABattleCharacter::ShowCharacterOutline(ECharacterOutlineType OutlineType)
{
	UMeshComponent* TempMesh = GetMesh();
	if (IsValid(TempMesh))
	{
		TempMesh->SetRenderCustomDepth((OutlineType != ECharacterOutlineType::None));
		TempMesh->SetCustomDepthStencilValue((int32)OutlineType);
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
	AttackBehavior->OnAnimNotifyAttackStart();
	DodgeBehavior->LockComponent(this);

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
	AttackBehavior->OnAnimNotifyAttackEnd();
	DodgeBehavior->UnlockComponent(this);

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

void ABattleCharacter::OnWeaponHit(AWeapon* Weapon, const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& HitResult : HitResults)
	{
		if (HitResult.Actor == this) {
			continue;
		}

		ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(HitResult.Actor);
		if (BattleCharacter)
		{
			FPointDamageEvent DamageEvent;
			DamageEvent.HitInfo = HitResult;
			BattleCharacter->TakeDamage(10, DamageEvent, GetController(), Weapon);
		}
	}
}

void ABattleCharacter::ExecuteAttack()
{
	AttackBehavior->RegisterCombo();
}

bool ABattleCharacter::ExecuteDodge(const FVector& Direction)
{
	if (DodgeBehavior->Dodge(Direction))
	{
		AttackBehavior->CancleAttack();
		TargetSelector->LockComponent(this);
		return true;
	}
	return false;
}

void ABattleCharacter::OnDodgeFinished(ACharacter* DodgingActor)
{
	if (DodgingActor == this)
	{
		TargetSelector->UnlockComponent(this);
	}
}

float ABattleCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bCanTkeDamage) {
		return 0;
	}

	float RealDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (RealDamageAmount <= 0) {
		return 0;
	}

	PlayDamageMontage(DamageEvent, DamageCauser);
	PlayHitFX(DamageEvent, DamageCauser);

	bCanTkeDamage = false;
	GetWorld()->GetTimerManager().SetTimer(
		InvincibleFrameHandle,
		this,
		&ABattleCharacter::OnInvincibleFrameEnd,
		InvincibleFrame,
		false
	);

	return RealDamageAmount;
}

void ABattleCharacter::PlayDamageMontage(FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	AWeapon* AttackWeapon = Cast<AWeapon>(DamageCauser);
	if (!IsValid(AttackWeapon)) {
		return;
	}

	TArray<UAnimMontage*> const* DamageMontageList = &FrontalDamageMontages;

	const AActor* AttackCharacter = AttackWeapon->GetOwner();
	const FVector AttackDirection = (AttackCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	const FVector OrientedDirection =
		GetActorForwardVector().Rotation().GetInverse().RotateVector(AttackDirection);
	const float AbsX = FMath::Abs(OrientedDirection.X);
	const float AbsY = FMath::Abs(OrientedDirection.Y);

	UE_LOG(LogTemp, Log, TEXT("OrientedDirection { X=%.2f, Y=%.2f }"), OrientedDirection.X, OrientedDirection.Y);
	if (AbsX > AbsY)
	{
		DamageMontageList = (OrientedDirection.X >= 0 ? &FrontalDamageMontages : &RearDamageMontages);
	}
	else
	{
		DamageMontageList = (OrientedDirection.Y >= 0 ? &RightDamageMontages : &LeftDamageMontages);
	}

	if (DamageMontageList->Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot find a suitable damage montage!"));
		return;
	}

	AnimInstance->Montage_Stop(0.1f, nullptr);

	int32 Index = FMath::RandRange(0, (int32)DamageMontageList->Num() - 1);
	UAnimMontage* DamageMontage = (*DamageMontageList)[Index];
	AnimInstance->Montage_Play(DamageMontage, 1.0f);
}

void ABattleCharacter::PlayHitFX(FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const auto& PointDamage = static_cast<const FPointDamageEvent&>(DamageEvent);
		AWeapon* HitWeapon = Cast<AWeapon>(DamageCauser);

		if (IsValid(HitWeapon))
		{
			const FHitVFX& HitVFX = HitWeapon->GetHitVFX();
			const FHitResult& HitInfo = PointDamage.HitInfo;
			FRotator Orientation = (HitInfo.TraceEnd - HitInfo.TraceStart).Rotation();
			//FRotator Orientation = HitInfo.Normal.Rotation();
			UNiagaraSystem* VFX = const_cast<UNiagaraSystem*>(HitVFX.ParticleTemplate);

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				VFX,
				PointDamage.HitInfo.Location,
				//Orientation + HitVFX.Orientation,
				HitVFX.Orientation,
				HitVFX.Scale
			);
		}
	}
}

void ABattleCharacter::OnInvincibleFrameEnd()
{
	bCanTkeDamage = true;
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

	const FName* SocketName = EquipSockets.Find(EquipType);
	if (SocketName) {
		Equipment->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			*SocketName
		);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Invalid equipment socket of type=%d"), (int32)EquipType);
	}

	Equipment->SetOwner(this);
	EquipmentList.Add(EquipType, Equipment);

	switch (Equipment->GetEquipType())
	{
	case EEquipmentType::Weapon:
		Equip_Weapon(Equipment);
		break;
	default:
		break;
	}

	if (bUpdateStats) {
		UpdateStats();
	}
}

void ABattleCharacter::Unequip(AEquipment* Equipment, bool bDestroy, bool bUpdateStats)
{
	if (!IsValid(Equipment) || Equipment->GetEquipType() == EEquipmentType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid equipment!"));
		return;
	}

	const EEquipmentType EquipType = Equipment->GetEquipType();
	AEquipment* CurrentEquipment = EquipmentList.FindOrAdd(EquipType);
	if (IsValid(CurrentEquipment) && CurrentEquipment == Equipment)
	{
		Equipment->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquipmentList.Remove(EquipType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot unequip an invalid equipment!"));
	}

	if (bUpdateStats) {
		UpdateStats();
	}

	switch (Equipment->GetEquipType())
	{
	case EEquipmentType::Weapon:
		Unequip_Weapon(Equipment);
		break;
	default:
		break;
	}

	Equipment->SetOwner(nullptr);
	if (bDestroy) {
		Equipment->Destroy();
	}
}

void ABattleCharacter::Equip_Weapon(AEquipment* Equipment)
{
	AWeapon* Weapon = Cast<AWeapon>(Equipment);
	if (IsValid(Weapon))
	{
		WeaponHitDelegate = Weapon->OnTraceHit.AddUObject(this, &ABattleCharacter::OnWeaponHit);
	}
}

void ABattleCharacter::Unequip_Weapon(AEquipment* Equipment)
{
	AWeapon* Weapon = Cast<AWeapon>(Equipment);
	if (IsValid(Weapon))
	{
		Weapon->OnTraceHit.Remove(WeaponHitDelegate);
	}
}

void ABattleCharacter::SelectTarget(AActor* NextTarget)
{
	TargetSelector->SelectTarget(NextTarget);
}

AActor* ABattleCharacter::GetSelectedTarget() const
{
	return TargetSelector->GetSelectedTarget();
}
