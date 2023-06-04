// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

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

	Stats.Hp = 100;
	Stats.MaxHp = 100;
	Stats.Atk = 10;
	Stats.Def = 10;
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

const FCharacterStats& ABattleCharacter::GetStats() const
{
	return Stats;
}

void ABattleCharacter::SetStats(const FCharacterStats& NewStats, bool bUpdateStats)
{
	Stats = NewStats;
	if (bUpdateStats) {
		UpdateStats();
	}
}

float ABattleCharacter::CalculateDamage(ABattleCharacter* Attacker, ABattleCharacter* Defender)
{
	if (!IsValid(Attacker) || !IsValid(Defender)) {
		return 0;
	}

	float Damage = ((4.0f * Attacker->Stats.Atk) - Defender->Stats.Def) / (1 + Defender->Stats.Def);
	Damage = FMath::Max(1.f, Damage);
	return Damage;
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
	if (IsDead()) {
		return;
	}

	FAttackData CurrentAttack = AttackBehavior->GetCurrentAttack();

	for (const FHitResult& HitResult : HitResults)
	{
		if (HitResult.Actor == this) {
			continue;
		}

		float RealDamageValue = 0;
		FMeleeDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.AttackData = CurrentAttack;

		ABattleCharacter* HitCharacter = Cast<ABattleCharacter>(HitResult.Actor);
		if (HitCharacter)
		{
			DamageEvent.Damage = CalculateDamage(this, HitCharacter);
			RealDamageValue = HitCharacter->TakeDamage(DamageEvent.Damage, DamageEvent, GetController(), Weapon);
		}

		if (CurrentAttack.HitStop > 0 && RealDamageValue > 0) {
			StartHitStop(CurrentAttack.HitStop, CurrentAttack.HitStopTimeDilation);
		}

		OnAttackHitDelegate.Broadcast(this, DamageEvent);
	}
}

void ABattleCharacter::ExecuteAttack()
{
	if (IsDead()) {
		return;
	}
	AttackBehavior->ComboAttack();
}

bool ABattleCharacter::ExecuteDodge(const FVector& Direction)
{
	if (IsDead()) {
		return false;
	}

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
	if (!bCanTakeDamage || IsDead()) {
		return 0;
	}

	float RealDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (RealDamageAmount <= 0) {
		return 0;
	}

	// Hit-stop
	float HitStop = 0;
	float HitStopTimeDilation = 0;
	// Knockback
	FVector2D KnockbackDirection = FVector2D::ZeroVector;
	float KnockbackSpeed = 0;

	if (DamageEvent.IsOfType(MELEE_DAMAGE_EVENT_CLASS_ID))
	{
		const FMeleeDamageEvent& MeleeDamageEvent = static_cast<const FMeleeDamageEvent&>(DamageEvent);
		const FAttackData& AttackData = MeleeDamageEvent.AttackData;

		HitStop = AttackData.HitStop;
		HitStopTimeDilation = AttackData.HitStopTimeDilation;
		KnockbackDirection = AttackData.KnockbackDirection;
		KnockbackSpeed = AttackData.KnockbackSpeed;
		UE_LOG(LogTemp, Log, TEXT("HitStop=%.2f HitStopTimeDilation=%.2f"), HitStop, HitStopTimeDilation);
		
		OnTakeMeleeDamageDelegate.Broadcast(this, MeleeDamageEvent, RealDamageAmount);
	}

	if (HitStop > 0) {
		StartHitStop(HitStop, HitStopTimeDilation);
	}

	Stats.Hp -= (int32)RealDamageAmount;

	if (Stats.Hp <= 0)
	{
		OnDead(DamageEvent, DamageCauser);
	}
	else
	{
		// Knockback's implementation is on held until a better solution has come up.
		/*if (KnockbackSpeed > 0)
		{
			AActor* Attacker = IsValid(DamageCauser) ? DamageCauser->GetOwner() : nullptr;
			Knockback(Attacker, KnockbackDirection, KnockbackSpeed);
		}
		else*/
		{
			PlayDamageMontage(DamageEvent, DamageCauser);
		}
	}

	PlayHitFX(DamageEvent, DamageCauser);

	bCanTakeDamage = false;
	GetWorld()->GetTimerManager().SetTimer(
		InvincibleFrameHandle,
		this,
		&ABattleCharacter::OnInvincibleFrameEnd,
		InvincibleFrame,
		false
	);

	return RealDamageAmount;
}

void ABattleCharacter::StartHitStop(float Seconds, float TimeDilation)
{
	CustomTimeDilation = TimeDilation;
	GetWorld()->GetTimerManager().SetTimer(
		HitStopHandle,
		this,
		&ABattleCharacter::EndHitStop,
		Seconds,
		false
	);
}

void ABattleCharacter::EndHitStop()
{
	CustomTimeDilation = 1;
	GetWorld()->GetTimerManager().ClearTimer(HitStopHandle);
}

bool ABattleCharacter::IsDead() const
{
	return Stats.Hp <= 0;
}

void ABattleCharacter::PlayDeadMontage()
{
	AnimInstance->Montage_Stop(0.1f, nullptr);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
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
	if (DamageEvent.IsOfType(FMeleeDamageEvent::ClassID))
	{
		const auto& MeleeDamage = static_cast<const FMeleeDamageEvent&>(DamageEvent);
		AWeapon* HitWeapon = Cast<AWeapon>(DamageCauser);
		const FAttackData& AttackData = MeleeDamage.AttackData;

		if (IsValid(HitWeapon))
		{
			const FHitVFX& HitVFX = HitWeapon->GetHitVFX();
			const FHitResult& HitInfo = MeleeDamage.HitInfo;
			FRotator Orientation = (HitInfo.TraceEnd - HitInfo.TraceStart).Rotation();
			//FRotator Orientation = HitInfo.Normal.Rotation();
			UNiagaraSystem* VFX = const_cast<UNiagaraSystem*>(HitVFX.ParticleTemplate);

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				VFX,
				MeleeDamage.HitInfo.Location,
				//Orientation + HitVFX.Orientation,
				HitVFX.Orientation,
				HitVFX.Scale
			);
		}

		if (IsValid(AttackData.CameraShakeFX))
		{
			UGameplayStatics::PlayWorldCameraShake(
				this,
				AttackData.CameraShakeFX,
				GetActorLocation(),
				0,
				500.f
			);
		}
	}
}

void ABattleCharacter::OnInvincibleFrameEnd()
{
	bCanTakeDamage = true;
}

void ABattleCharacter::Knockback(AActor* Attacker, const FVector2D& RelativeDirection, float Speed)
{
	FVector KnockbackDirection = ConmputeKnockbackDirection(Attacker, RelativeDirection);
	LaunchCharacter(KnockbackDirection.GetSafeNormal() * Speed, true, false);

	UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (IsValid(MoveComp))
	{
		MoveComp->GroundFriction = 0;
	}

	bKnockback = true;

	GetWorld()->GetTimerManager().SetTimer(
		CheckKnockbackTimerHandle,
		this,
		&ABattleCharacter::CheckKnockbackEnd,
		0.1f, true, 0.1f
	);
}

FVector ABattleCharacter::ConmputeKnockbackDirection(AActor* Attacker, const FVector2D& RelativeDirection)
{
	FVector KnockbackDirection = { RelativeDirection.X, 0, RelativeDirection.Y };

	if (IsValid(Attacker))
	{
		FVector PushVector = (GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal2D();
		FRotator PushRotation = PushVector.Rotation();
		KnockbackDirection = PushRotation.RotateVector(KnockbackDirection).GetSafeNormal();
	}
	else
	{
		KnockbackDirection = -GetActorForwardVector();
		KnockbackDirection.Z = FMath::Tan(FMath::DegreesToRadians(30.f));
		KnockbackDirection.Normalize();
	}

	return KnockbackDirection;
}

void ABattleCharacter::CheckKnockbackEnd()
{
	const auto CharMovement = GetCharacterMovement();
	bool bClearTimer = true;

	if (!IsDead() && IsValid(CharMovement))
	{
		if (!CharMovement->IsFalling())
		{
			CharMovement->GroundFriction = 8.0f;
			bKnockback = false;
		}
		else
		{
			bClearTimer = false;
		}
	}

	if (bClearTimer) {
		GetWorld()->GetTimerManager().ClearTimer(CheckKnockbackTimerHandle);
	}
}

void ABattleCharacter::OnDead(FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	OnDeadDelegate.Broadcast(this);

	AttackBehavior->LockComponent(this);
	DodgeBehavior->LockComponent(this);

	TargetSelector->SelectTarget(nullptr);
	TargetSelector->LockComponent(this);

	SetCanBeDamaged(false);

	if (IsValid(DeadMontage))
	{
		PlayDeadMontage();
	}
	else
	{
		AActor* Attacker = IsValid(DamageCauser) ? DamageCauser->GetOwner() : nullptr;
		FVector2D KnockbackDirection{
			FMath::Cos(FMath::DegreesToRadians(30.f)),
			FMath::Sin(FMath::DegreesToRadians(30.f))
		};
		Knockback(Attacker, KnockbackDirection, DeadKnockbackSpeed);
		AnimInstance->Montage_Stop(0.1f, nullptr);
	}
}

void ABattleCharacter::Equip(AEquipment* Equipment, bool bUpdateStats)
{
	if (IsDead()) {
		return;
	}

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
	if (IsDead()) {
		return;
	}

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
	if (IsDead()) {
		return;
	}

	TargetSelector->SelectTarget(NextTarget);
}

AActor* ABattleCharacter::GetSelectedTarget() const
{
	return TargetSelector->GetSelectedTarget();
}

bool ABattleCharacter::IsBeingTargetted() const
{
	return TargetSelector->GetActorTargettingOwner() != nullptr;
}
