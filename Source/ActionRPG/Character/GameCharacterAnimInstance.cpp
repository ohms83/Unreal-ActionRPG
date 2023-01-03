// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterAnimInstance.h"

DEFINE_LOG_CATEGORY(LogGameCharacterAnimInstance);

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
