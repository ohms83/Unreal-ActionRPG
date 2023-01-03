// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacterController.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"
#include "ActionRPG/Component/DodgeBehavior.h"

void ABattleCharacterController::BeginPlay()
{
    const auto aPawn = GetPawn();
    if (!IsValid(aPawn)) {
        return;
    }

    DodgeBehavior = Cast<UDodgeBehavior>(aPawn->GetComponentByClass(UDodgeBehavior::StaticClass()));
}

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
    if (IsValid(DodgeBehavior) && DodgeBehavior->Dodge()) {
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
