// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionRPG/Controller/ThirdPersonController.h"
#include "TestThirdPersonController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API ATestThirdPersonController : public AThirdPersonController
{
	GENERATED_BODY()
	
protected:
	// Test whether the event will invoke the overriden or based functions.
	virtual void OnAnimationStateEnter(const FString& AnimStateName) override;
};
