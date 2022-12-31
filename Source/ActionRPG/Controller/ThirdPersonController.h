// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "ThirdPersonController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogThirdPersonController, Log, All);

class UGameCharacterAnimInstance;

/**
 * 
 */
UCLASS()
class ACTIONRPG_API AThirdPersonController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

	virtual void OnInputAxisMoveForward(float AxisValue);
	virtual void OnInputAxisMoveRight(float AxisValue);
	virtual void OnInputTurn(float AxisValue);
	virtual void OnInputLookUp(float AxisValue);
	virtual void OnInputTurnRate(float AxisValue);
	virtual void OnInputLookUpRate(float AxisValue);
	virtual void OnInputActionJump();
	virtual void OnInputActionStopJumping();

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	virtual void OnAnimationStateEnter(const FString& AnimStateName);
	virtual void OnAnimationStateExit(const FString& AnimStateName);
	virtual void OnAnimationStateFullyBlend(const FString& AnimStateName);

	const UGameCharacterAnimInstance* GetAnimInstance() const { return AnimInstance; }
	UGameCharacterAnimInstance* GetAnimInstance() { return AnimInstance; }

	const UCharacterMovementComponent* GetMoveComponent() const { return CharacterMovementComp; }
	UCharacterMovementComponent* GetMoveComponent() { return CharacterMovementComp; }

private:
	void InitAnimInstance(APawn* aPawn);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName MoveForwardEventName = "MoveForward";
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName MoveRightEventName = "MoveRight";
	// Camera's turn event
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName TurnEventName = "Turn";
	// Camera's look up event
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName LookUpEventName = "LookUp";
	// Camera's turn event (for analog stick)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName TurnRateEventName = "TurnRate";
	// Camera's look up event (for analog stick)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName LookUpRateEventName = "LookUpRate";
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FName JumpEventName = "Jump";

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FTransform CameraTransform;
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float SprintArmLength = 400.f;

	UPROPERTY(EditAnyWhere, Category = "Control")
	float BaseTurnRate = 45;
	UPROPERTY(EditAnyWhere, Category = "Control")
	float BaseLookUpRate = 45;

	//ACharacter* ControlledChar = nullptr;
	USpringArmComponent* CameraBoom = nullptr;
	UCameraComponent* FollowCamera = nullptr;
	UCharacterMovementComponent* CharacterMovementComp = nullptr;

	// Animation
	UGameCharacterAnimInstance* AnimInstance = nullptr;
	FDelegateHandle AnimEnterStateDelegateHandles;
};
