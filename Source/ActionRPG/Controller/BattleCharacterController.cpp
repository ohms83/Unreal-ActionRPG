// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacterController.h"
#include "ActionRPG/Character/BattleCharacter.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"
#include "ActionRPG/Component/AttackBehavior.h"
#include "ActionRPG/Component/DodgeBehavior.h"
#include "Kismet/KismetMathLibrary.h"

void ABattleCharacterController::BeginPlay()
{
    const auto aPawn = GetPawn();
    if (!IsValid(aPawn)) {
        return;
    }

    AttackBehavior = Cast<UAttackBehavior>(aPawn->GetComponentByClass(UAttackBehavior::StaticClass()));
    DodgeBehavior = Cast<UDodgeBehavior>(aPawn->GetComponentByClass(UDodgeBehavior::StaticClass()));
}

void ABattleCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (IsValid(InputComponent))
    {
        InputComponent->BindAction
        (
            AttackEventName,
            IE_Pressed,
            this,
            &ABattleCharacterController::OnInputActionAttack
        );

        InputComponent->BindAction
        (
            DodgeEventName,
            IE_Pressed,
            this,
            &ABattleCharacterController::OnInputActionDodge
        );
        InputComponent->BindAction
        (
            DodgeEventName,
            IE_Released,
            this,
            &ABattleCharacterController::OnInputActionStopDodging
        );
    }
}

void ABattleCharacterController::OnInputActionAttack()
{
    if (IsInputLocked(EInputLockFlag::Action)) {
        return;
    }

    ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(GetPawn());
    if (IsValid(BattleCharacter))
    {
        BattleCharacter->ExecuteAttack();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Controlled pawn is not an ABattleCharacter type!"));
    }
}

void ABattleCharacterController::OnInputActionDodge()
{
    if (IsInputLocked(EInputLockFlag::Action) || IsInputLocked(EInputLockFlag::Movement)) {
        return;
    }

    const FRotator Rotator(0, GetControlRotation().Yaw, 0);
    const FVector2D& TempInputAxis = GetInputAxis();
    const float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(TempInputAxis.X, TempInputAxis.Y));
    const FVector Direction = TempInputAxis.IsZero() ? FVector::ZeroVector :
        UKismetMathLibrary::RotateAngleAxis(Rotator.Vector(), AngleDeg, FVector::ZAxisVector);


    ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(GetPawn());
    if (IsValid(BattleCharacter) && BattleCharacter->ExecuteDodge(Direction))
    {
        LockInput(EInputLockFlag::Movement);
        LockInput(EInputLockFlag::Action);
    }

    InputActionDodge_Bluerprint();
}

void ABattleCharacterController::OnInputActionStopDodging()
{

}

void ABattleCharacterController::OnAnimationStateEnter(const FString& AnimStateName)
{
    Super::OnAnimationStateEnter(AnimStateName);
}
