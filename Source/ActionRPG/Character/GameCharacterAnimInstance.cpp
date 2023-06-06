// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogGameCharacterAnimInstance);

void UGameCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* PawnOwner = TryGetPawnOwner();
	if (!IsValid(PawnOwner)) {
		return;
	}

	auto MoveComp = PawnOwner->GetMovementComponent();
	if (IsValid(MoveComp))
	{
		MoveSpeed = MoveComp->Velocity.Size();
		bIsFalling = MoveComp->IsFalling();
		PawnMoveDirection = MoveComp->Velocity.GetSafeNormal();
	}

	PawnOrientation = PawnOwner->GetActorRotation();
	//PawnMoveDirection = PawnOwner->GetLastMovementInputVector();
}

void UGameCharacterAnimInstance::BroadcastOnEnterState(const FString& StateName)
{
	StateEnterEvent.Broadcast(StateName);
}

void UGameCharacterAnimInstance::BroadcastOnExitState(const FString& StateName)
{
	StateExitEvent.Broadcast(StateName);
}

void UGameCharacterAnimInstance::BroadcastOnFullyBlendState(const FString& StateName)
{
	StateFullyBlendEvent.Broadcast(StateName);
}
