// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacterController.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"

void ABattleCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (IsValid(InputComponent))
    {
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

void ABattleCharacterController::OnInputActionDodge()
{
    const auto TempMoveComp = GetMoveComponent();
    if (!IsValid(TempMoveComp) || TempMoveComp->IsFalling()) {
        return;
    }

    const auto TempAnimInstance = GetAnimInstance();
    if (IsValid(TempAnimInstance)) {
        TempAnimInstance->PlayDodgeMontage();
        DisableInput(this);
    }

    InputActionDodge_Bluerprint();
}

void ABattleCharacterController::OnInputActionStopDodging()
{

}

void ABattleCharacterController::OnAnimationStateEnter(const FString& AnimStateName)
{
    Super::OnAnimationStateEnter(AnimStateName);

    if (AnimStateName.Equals(TEXT("idle"), ESearchCase::IgnoreCase))
    {
        EnableInput(this);
    }
}
