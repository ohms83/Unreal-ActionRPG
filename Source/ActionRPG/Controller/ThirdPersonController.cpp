// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonController.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "ActionRPG/Character/GameCharacterAnimInstance.h"

DEFINE_LOG_CATEGORY(LogThirdPersonController);

void AThirdPersonController::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (IsValid(InputComponent))
    {
        InputComponent->BindAxis
        (
            MoveForwardEventName,
            this,
            &AThirdPersonController::OnInputAxisMoveForward
        );
        InputComponent->BindAxis
        (
            MoveRightEventName,
            this,
            &AThirdPersonController::OnInputAxisMoveRight
        );
        InputComponent->BindAxis
        (
            TurnEventName,
            this,
            &AThirdPersonController::OnInputTurn
        );
        InputComponent->BindAxis
        (
            LookUpEventName,
            this,
            &AThirdPersonController::OnInputLookUp
        );
        InputComponent->BindAxis
        (
            TurnRateEventName,
            this,
            &AThirdPersonController::OnInputTurnRate
        );
        InputComponent->BindAxis
        (
            LookUpRateEventName,
            this,
            &AThirdPersonController::OnInputLookUpRate
        );

        InputComponent->BindAction
        (
            JumpEventName,
            IE_Pressed,
            this,
            &AThirdPersonController::OnInputActionJump
        );
        InputComponent->BindAction
        (
            JumpEventName,
            IE_Released,
            this,
            &AThirdPersonController::OnInputActionStopJumping
        );
    }
}

void AThirdPersonController::LockInput(EInputLockFlag LockFlag)
{
    InputLockFlag |= (uint8)LockFlag;
}

void AThirdPersonController::UnlockInput(EInputLockFlag LockFlag)
{
    InputLockFlag &= ~(uint8)(LockFlag);
}

bool AThirdPersonController::IsInputLocked(EInputLockFlag LockFlag) const
{
    return InputLockFlag & (uint8)LockFlag;
}

void AThirdPersonController::InitAnimInstance(APawn* aPawn)
{
    const auto TempCharacter = Cast<ACharacter>(aPawn);
    if (!IsValid(TempCharacter)) {
        return;
    }

    const auto Mesh = TempCharacter->GetMesh();
    if (!IsValid(Mesh)) {
        return;
    }

    AnimInstance = Cast<UGameCharacterAnimInstance>(Mesh->GetAnimInstance());
    if (!IsValid(AnimInstance)) {
        return;
    }

    AnimEnterStateDelegateHandles = AnimInstance->OnEnterState().AddUObject(
        this, &AThirdPersonController::OnAnimationStateEnter);
    AnimExitStateDelegateHandles = AnimInstance->OnExitState().AddUObject(
        this, &AThirdPersonController::OnAnimationStateExit);
    AnimFullyBlendStateDelegateHandles = AnimInstance->OnFullyBlendState().AddUObject(
        this, &AThirdPersonController::OnAnimationStateFullyBlend);
}

void AThirdPersonController::OnInputAxisMoveForward(float AxisValue)
{
    if (IsInputLocked(EInputLockFlag::Movement)) {
        return;
    }

    FRotator Rotator(0, GetControlRotation().Yaw, 0);
    FVector Direction = UKismetMathLibrary::GetForwardVector(Rotator);

    if (IsValid(AnimInstance))
    {
        InputAxis.Y = AxisValue;
        AnimInstance->InputAxis.Y = AxisValue;
    }

    GetPawn()->AddMovementInput(Direction, AxisValue);
}

void AThirdPersonController::OnInputAxisMoveRight(float AxisValue)
{
    if (IsInputLocked(EInputLockFlag::Movement)) {
        return;
    }

    FRotator Rotator(0, GetControlRotation().Yaw, 0);
    FVector Direction = UKismetMathLibrary::GetRightVector(Rotator);

    if (IsValid(AnimInstance)) 
    {
        InputAxis.X = AxisValue;
        AnimInstance->InputAxis.X = AxisValue;
    }

    GetPawn()->AddMovementInput(Direction, AxisValue);
}

void AThirdPersonController::OnInputTurn(float AxisValue)
{
    if (IsInputLocked(EInputLockFlag::Camera)) {
        return;
    }

    AddYawInput(AxisValue);
}

void AThirdPersonController::OnInputLookUp(float AxisValue)
{
    if (IsInputLocked(EInputLockFlag::Camera)) {
        return;
    }

    AddPitchInput(AxisValue);
}

void AThirdPersonController::OnInputTurnRate(float AxisValue)
{
    if (IsInputLocked(EInputLockFlag::Camera)) {
        return;
    }

    float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);
    float Yaw = DeltaSeconds * BaseTurnRate * AxisValue;
    AddYawInput(Yaw);
}

void AThirdPersonController::OnInputLookUpRate(float AxisValue)
{
    if (IsInputLocked(EInputLockFlag::Camera)) {
        return;
    }

    float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);
    float Pitch = DeltaSeconds * BaseLookUpRate * AxisValue;
    AddPitchInput(Pitch);
}

void AThirdPersonController::OnInputActionJump()
{
    if (IsInputLocked(EInputLockFlag::Movement)) {
        return;
    }

    ACharacter* TempCharacter = GetCharacter();
    if (IsValid(TempCharacter)) {
        TempCharacter->Jump();
    }
}

void AThirdPersonController::OnInputActionStopJumping()
{
    ACharacter* TempCharacter = GetCharacter();
    if (IsValid(TempCharacter)) {
        TempCharacter->StopJumping();
    }
}

void AThirdPersonController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    CameraBoom = Cast<USpringArmComponent>(
        aPawn->AddComponentByClass(USpringArmComponent::StaticClass(), false, CameraTransform, false));
    CameraBoom->TargetArmLength = SprintArmLength;
    aPawn->AddInstanceComponent(CameraBoom);

    FollowCamera = Cast<UCameraComponent>(
        aPawn->AddComponentByClass(UCameraComponent::StaticClass(), false, FTransform{}, false));
    FollowCamera->AttachToComponent(
        CameraBoom,
        FAttachmentTransformRules {
            EAttachmentRule::KeepRelative,
            false
        }
    );
    aPawn->AddInstanceComponent(FollowCamera);

    CharacterMovementComp = Cast<UCharacterMovementComponent>(
        aPawn->GetComponentByClass(UCharacterMovementComponent::StaticClass()));

    // Third person camera control
    CameraBoom->bUsePawnControlRotation = true;
    aPawn->bUseControllerRotationYaw = false;
    
    if (IsValid(CharacterMovementComp))
    {
        CharacterMovementComp->bOrientRotationToMovement = true;
    }

    InitAnimInstance(aPawn);
}

void AThirdPersonController::OnUnPossess()
{
    Super::OnUnPossess();

    if (IsValid(CameraBoom))
    {
        CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        CameraBoom = nullptr;
    }
    if (IsValid(FollowCamera))
    {
        FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        FollowCamera = nullptr;
    }

    CharacterMovementComp = nullptr;

    if (IsValid(AnimInstance))
    {
        AnimInstance->OnEnterState().Remove(AnimEnterStateDelegateHandles);
        AnimInstance->OnExitState().Remove(AnimExitStateDelegateHandles);
        AnimInstance->OnFullyBlendState().Remove(AnimFullyBlendStateDelegateHandles);
    }
}

void AThirdPersonController::OnAnimationStateEnter(const FString& AnimStateName)
{
    //UE_LOG(LogThirdPersonController, Log, TEXT("Enter Anim State=%s"), *AnimStateName);
    if (AnimStateName.Equals(TEXT("idle"), ESearchCase::IgnoreCase)) {
        UnlockInput(EInputLockFlag::Movement);
    }
}
void AThirdPersonController::OnAnimationStateExit(const FString& AnimStateName)
{

}
void AThirdPersonController::OnAnimationStateFullyBlend(const FString& AnimStateName)
{
    //UE_LOG(LogThirdPersonController, Log, TEXT("Fully Blend Anim State=%s"), *AnimStateName);
    if (AnimStateName.Equals(TEXT("jumpend"), ESearchCase::IgnoreCase)) {
        LockInput(EInputLockFlag::Movement);
    }
}
