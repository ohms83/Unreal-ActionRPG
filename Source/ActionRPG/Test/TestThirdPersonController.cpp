// Fill out your copyright notice in the Description page of Project Settings.


#include "TestThirdPersonController.h"

void ATestThirdPersonController::OnAnimationStateEnter(const FString& AnimStateName)
{
	Super::OnAnimationStateEnter(AnimStateName);
	UE_LOG(LogThirdPersonController, Log, TEXT("ATestThirdPersonController::OnAnimationStateEnter"));
}