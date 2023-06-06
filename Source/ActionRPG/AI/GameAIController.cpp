// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAIController.h"
#include "Navigation/CrowdFollowingComponent.h"

AGameAIController::AGameAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void AGameAIController::ChangeState(EAIState NextState)
{
	State = NextState;
}
