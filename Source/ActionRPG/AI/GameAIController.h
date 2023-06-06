// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameAIController.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	None		UMETA(DisplayName = "None"),
	// Searching for enemy
	Searching	UMETA(DisplayName = "Searching"),
	// Move to the target
	MoveTo		UMETA(DisplayName = "Move To"),
	// Stadnby for attack
	Standby		UMETA(DisplayName = "Standby"),
	// Executing attack to the target enemy
	Attack		UMETA(DisplayName = "Attack"),
};

/**
 * 
 */
UCLASS()
class ACTIONRPG_API AGameAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGameAIController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Game AI Controller")
	EAIState GetState() const { return State; }
	UFUNCTION(BlueprintCallable, Category = "Game AI Controller")
	void ChangeState(EAIState NextState);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Game AI Controller")
	EAIState State = EAIState::None;
};
