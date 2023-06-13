// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacterController.h"
#include "ActionRPG/Character/BattleCharacter.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"
#include "ActionRPG/Component/AttackBehavior.h"
#include "ActionRPG/Component/DodgeBehavior.h"
#include "ActionRPG/Component/TargetSelectorComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ABattleCharacterController::BeginPlay()
{
    AttackBehavior = GetPawnComponent<UAttackBehavior>();
    DodgeBehavior = GetPawnComponent<UDodgeBehavior>();
    TargetSelector = GetPawnComponent<UTargetSelectorComponent>();

    ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(GetPawn());
    if (IsValid(BattleCharacter))
    {
        FScriptDelegate FlashMoveBeginDelegate;
        FlashMoveBeginDelegate.BindUFunction(this, "OnSpecialMoveTriggered");
        FScriptDelegate FlashMoveEndDelegate;
        FlashMoveEndDelegate.BindUFunction(this, "OnSpecialMoveEnded");

        BattleCharacter->OnFlashMoveDynamicDelegate.Add(FlashMoveBeginDelegate);
        BattleCharacter->OnFlashMoveEndDynamicDelegate.Add(FlashMoveEndDelegate);
    }
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
            SpecialAttackEventName,
            IE_Pressed,
            this,
            &ABattleCharacterController::OnInputActionAttackSpecial
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

        InputComponent->BindAction
        (
            SelectNextTargetEventName,
            IE_Released,
            this,
            &ABattleCharacterController::OnInputActionSelectNextTarget
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
        if (bOneshotSpecialAttack)
        {
            const bool bHastarget = TargetSelector ? TargetSelector->GetSelectedTarget() != nullptr : false;
            if (!bHastarget) {
                TargetSelector->SelectNextTarget();
            }
            BattleCharacter->ExecuteSpecialAttack(true);
            bOneshotSpecialAttack = false;
        }
        else {
            BattleCharacter->ExecuteAttack();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Controlled pawn is not an ABattleCharacter type!"));
    }
}

void ABattleCharacterController::OnInputActionAttackSpecial()
{
    if (IsInputLocked(EInputLockFlag::Action)) {
        return;
    }

    ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(GetPawn());
    if (IsValid(BattleCharacter))
    {
        BattleCharacter->ExecuteSpecialAttack();
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

void ABattleCharacterController::OnInputActionSelectNextTarget()
{
    if (IsValid(TargetSelector)) {
        TargetSelector->SelectNextTarget();
    }
}

void ABattleCharacterController::OnAnimationStateEnter(const FString& AnimStateName)
{
    Super::OnAnimationStateEnter(AnimStateName);
}

void ABattleCharacterController::OnSpecialMoveTriggered(ABattleCharacter* BattleCharacter, float Countdown)
{
    bOneshotSpecialAttack = true;
}

void ABattleCharacterController::OnSpecialMoveEnded(ABattleCharacter* BattleCharacter, float Countdown)
{
    bOneshotSpecialAttack = false;
}
