// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCharacterController.h"
#include "ActionRPG/Character/GameCharacterAnimInstance.h"
#include "ActionRPG/Component/DodgeBehavior.h"
#include "Kismet/KismetMathLibrary.h"

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
    const FRotator Rotator(0, GetControlRotation().Yaw, 0);
    const FVector2D& TempInputAxis = GetInputAxis();
    const float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(TempInputAxis.X, TempInputAxis.Y));
    const FVector Direction = TempInputAxis.IsZero() ? FVector::ZeroVector :
        UKismetMathLibrary::RotateAngleAxis(Rotator.Vector(), AngleDeg, FVector::ZAxisVector);

    if (IsValid(DodgeBehavior) && DodgeBehavior->Dodge(Direction)) {
        LockInput(EInputLockFlag::Movement);
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
