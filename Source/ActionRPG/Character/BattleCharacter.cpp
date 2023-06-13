// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "ActionRPG/Component/AttackBehavior.h"
#include "ActionRPG/Component/DodgeBehavior.h"
#include "ActionRPG/Component/TargetSelectorComponent.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"

#include "ActionRPG/Command/TeleportAttackCommand.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Sound/SoundWave.h"

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

	/*OuterCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Outer Collision"));
	OuterCollision->InitCapsuleSize(60.0f, 100.0f);
	OuterCollision->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	OuterCollision->CanCharacterStepUpOn = ECB_No;
	OuterCollision->SetShouldUpdatePhysicsVolume(false);
	OuterCollision->SetCanEverAffectNavigation(false);
	OuterCollision->bDynamicObstacle = false;
	OuterCollision->SetupAttachment(GetRootComponent());*/
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
		AttackBehavior->CancelAttack();
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

void ABattleCharacter::SetTeam(ECharacterTeam NewTeam)
{
	Team = NewTeam;
}

bool ABattleCharacter::IsSameTeam(AActor* const OtherActor) const
{
	if (!IsValid(OtherActor)) {
		return false;
	}

	ABattleCharacter* OtherCharacter = Cast<ABattleCharacter>(OtherActor);
	if (!IsValid(OtherCharacter)) {
		OtherCharacter = Cast<ABattleCharacter>(OtherActor->GetOwner());
	}

	return IsValid(OtherCharacter) && OtherCharacter->GetTeam() == GetTeam();
}

float ABattleCharacter::CalculateDamage(ABattleCharacter* Attacker, ABattleCharacter* Defender, const FAttackData& AttackData)
{
	if (!IsValid(Attacker) || !IsValid(Defender)) {
		return 0;
	}

	float Damage = ((4.0f * Attacker->Stats.Atk) - Defender->Stats.Def) / (1 + Defender->Stats.Def) * AttackData.DamageModifier;
	Damage = FMath::Max(1.f, Damage);
	return Damage;
}

void ABattleCharacter::OnAnimNotifyAttackStart()
{
	if (AttackBehavior->IsLocked()) {
		return;
	}
	AttackBehavior->OnAnimNotifyAttackStart();
	DodgeBehavior->LockComponent(this);
	EnableTraceHit(true);
}

void ABattleCharacter::OnAnimNotifyAttackEnd()
{
	AttackBehavior->OnAnimNotifyAttackEnd();
	DodgeBehavior->UnlockComponent(this);
	EnableTraceHit(false);
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

		if (SpecialMoveFrameHandle.IsValid()) {
			CurrentAttack.HitStop = 0;
		}

		float RealDamageValue = 0;
		FMeleeDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.AttackData = CurrentAttack;

		ABattleCharacter* HitCharacter = Cast<ABattleCharacter>(HitResult.Actor);
		if (HitCharacter)
		{
			DamageEvent.Damage = CalculateDamage(this, HitCharacter, CurrentAttack);
			RealDamageValue = HitCharacter->TakeDamage(DamageEvent.Damage, DamageEvent, GetController(), Weapon);
		}

		if (CurrentAttack.HitStop > 0 && RealDamageValue > 0) {
			StartHitStop(CurrentAttack.HitStop, CurrentAttack.HitStopTimeDilation);
		}

		if (RealDamageValue > 0) {
			UpdateSpecialMoveGuage(0.05f);
		}

		OnAttackHitDelegate.Broadcast(this, DamageEvent);
	}
}

void ABattleCharacter::EnableTraceHit(bool bEnable)
{
	AEquipment** WeaponPtr = EquipmentList.Find(EEquipmentType::Weapon);
	if (WeaponPtr)
	{
		AWeapon* Weapon = Cast<AWeapon>(*WeaponPtr);
		if (IsValid(Weapon))
		{
			Weapon->EnableTraceHit(bEnable);
		}
	}
}

bool ABattleCharacter::ExecuteAttack()
{
	if (IsDead()) {
		return false;
	}
	return AttackBehavior->ComboAttack();
}

bool ABattleCharacter::ExecuteSpecialAttack(bool bForceActivation)
{
	if (IsDead()) {
		return false;
	}

	if (!bForceActivation && SpecialMoveGuage < 1.0f) {
		return false;
	}

	auto Command = NewObject<UTeleportAttackCommand>(this, TeleportAttackCommandClass);

	AActor* SelectedTarget = TargetSelector->GetSelectedTarget();
	if (SelectedTarget)
	{
		Command->TargetLocation = SelectedTarget->GetActorLocation() - (GetActorForwardVector() * 100.f);
	}
	else
	{
		Command->TargetLocation = GetActorLocation() + (GetActorForwardVector() * 500.f);
	}
	RegisterCommand(Command);

	if (!bForceActivation) {
		SpecialMoveGuage = 0;
	}

	return true;
}

bool ABattleCharacter::ExecuteDodge(const FVector& Direction)
{
	if (IsDead()) {
		return false;
	}

	if (DodgeBehavior->Dodge(Direction))
	{
		// Flash Move
		{
			FTimerDelegate FlashMoveTimerDelegate;
			FlashMoveTimerDelegate.BindLambda([]() {
			});
			SetTimer(
				FlashMoveActivationWindowHandle,
				FlashMoveTimerDelegate,
				FlashMoveActicationWindow,
				false
			);
		}

		AttackBehavior->CancelAttack();
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
	float RealDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (RealDamageAmount <= 0) {
		return 0;
	}
	
	// Detects special moves
	if (IsTimerActive(FlashMoveActivationWindowHandle))
	{
		TriggerFlashMove();
		return 0;
	}

	const FMeleeDamageEvent* MeleeDamageEvent = DamageEvent.IsOfType(MELEE_DAMAGE_EVENT_CLASS_ID) ?
		static_cast<const FMeleeDamageEvent*>(&DamageEvent) : nullptr;
	const FHitResult* HitInfo = (MeleeDamageEvent ? &MeleeDamageEvent->HitInfo : nullptr);

	const FAttackData* AttackData = (MeleeDamageEvent ? &MeleeDamageEvent->AttackData : nullptr);
	// Hit-stop
	float HitStop = (AttackData ? AttackData->HitStop : 0);
	float HitStopTimeDilation = (AttackData ? AttackData->HitStopTimeDilation : 0);
	// Knockback
	FVector2D KnockbackDirection = (AttackData ? AttackData->KnockbackDirection : FVector2D::ZeroVector);
	float KnockbackSpeed = (AttackData ? AttackData->KnockbackSpeed : 0);

	if (MeleeDamageEvent)
	{	
		OnTakeMeleeDamageDelegate.Broadcast(this, *MeleeDamageEvent, RealDamageAmount);
	}
	UE_LOG(LogTemp, Log, TEXT("HitStop=%.2f HitStopTimeDilation=%.2f"), HitStop, HitStopTimeDilation);

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

	AttackBehavior->TempLock(this, 0.1f);
	DodgeBehavior->TempLock(this, 0.1f);
	EnableTraceHit(false);

	bSpecialMoveFlag = false;

	ClearTimer(FlashMoveActivationWindowHandle);
	StartInvincibleFrame(InvincibleFrame);

	return RealDamageAmount;
}

bool ABattleCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return bCanTakeDamage &&
		!IsSameTeam(DamageCauser) &&
		!IsDead() &&
		Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ABattleCharacter::StartHitStop(float Seconds, float TimeDilation)
{
	if (!bHitStopEnabled) {
		return;
	}

	CustomTimeDilation = TimeDilation;
	//Seconds *= (bSpecialMoveFlag ? FlashMoveGlobalTimeDilation : 1.f);
	//UE_LOG(LogTemp, Log, TEXT("%s StartHitStop Seconds=%.2f"), *GetName(), Seconds);
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
	if (!bHitStopEnabled) {
		return;
	}

	CustomTimeDilation = 1;
	GetWorld()->GetTimerManager().ClearTimer(HitStopHandle);
}

void ABattleCharacter::StartInvincibleFrame(float Seconds)
{
	bCanTakeDamage = false;

	FTimerDelegate InvincibleFrameDelegate;
	InvincibleFrameDelegate.BindLambda([this]() {
		bCanTakeDamage = true;
	});
	SetTimer(
		InvincibleFrameHandle,
		InvincibleFrameDelegate,
		Seconds,
		false
	);
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
	AActor* Attacker = IsValid(DamageCauser) ? DamageCauser->GetOwner() : nullptr;

	OnDeadDelegate.Broadcast(this);
	OnCharacterDeadDynamicDelegate.Broadcast(this, Cast<ABattleCharacter>(Attacker), DamageEvent);
	OnDeadEvent_BP(DamageEvent, DamageCauser);

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

void ABattleCharacter::UnequipAll(bool bDestroy, bool bUpdateStats)
{
	while (EquipmentList.Num() > 0)
	{
		Unequip(EquipmentList.begin()->Value, bDestroy, bUpdateStats);
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

float ABattleCharacter::GetFlashMoveScaledCountdownTime() const
{
	return SpecialMoveFrame * FlashMoveGlobalTimeDilation;
}

void ABattleCharacter::UpdateSpecialMoveGuage(float AddValue)
{
	SpecialMoveGuage += AddValue;
	SpecialMoveGuage = FMath::Clamp<float>(SpecialMoveGuage, 0, 1);
}

void ABattleCharacter::TriggerFlashMove()
{
	bSpecialMoveFlag = true;

	FTimerDelegate FlashMoveTimerDelegate;
	FlashMoveTimerDelegate.BindLambda([this]() {
		OnSpecialMoveEnd();
		OnFlashMoveEndDynamicDelegate.Broadcast(this, 0);
	});
	float ScaledCountdownTime = GetFlashMoveScaledCountdownTime();
	GetWorld()->GetTimerManager().SetTimer(
		SpecialMoveFrameHandle,
		FlashMoveTimerDelegate,
		ScaledCountdownTime,
		false
	);
	StartInvincibleFrame(ScaledCountdownTime);

	ClearTimer(InvincibleFrameHandle);
	ClearTimer(FlashMoveActivationWindowHandle);

	UGameplayStatics::SetGlobalTimeDilation(this, FlashMoveGlobalTimeDilation);
	CustomTimeDilation = FlashMoveCharacterTimeDilation / FlashMoveGlobalTimeDilation;

	if (IsValid(FlashmoveVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			FlashmoveVFX,
			GetActorLocation(),
			//Orientation + HitVFX.Orientation,
			GetActorRotation(),
			FVector::OneVector
		);
	}

	if (IsValid(FlashMoveSFX))
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FlashMoveSFX,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	UpdateSpecialMoveGuage(0.5f);
	OnFlashMoveDynamicDelegate.Broadcast(this, SpecialMoveFrame);
}

void ABattleCharacter::TriggerParry()
{
}

void ABattleCharacter::OnSpecialMoveEnd()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
	CustomTimeDilation = 1.f;
	bCanTakeDamage = true;
	bSpecialMoveFlag = false;
}
