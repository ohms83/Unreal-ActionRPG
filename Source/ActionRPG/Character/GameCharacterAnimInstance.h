// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameCharacterAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameCharacterAnimInstance, Log, All);

/**
 * AGameCharacter's animation instance base class. All of the character animation BP should be
 * inherited from here.
 */
UCLASS()
class ACTIONRPG_API UGameCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Character's move speed using for Idle/Run blendspace
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Properties")
	float MoveSpeed = 0;
	// Flag indicating whether the character is falling.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Properties")
	bool bIsFalling = false;
	// Axis value from the input event.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Properties")
	FVector2D InputAxis;

	DECLARE_EVENT_OneParam(UGameCharacterAnimInstance, FStateEvent, const FString&);

	FStateEvent& OnEnterState()
	{
		return StateEnterEvent;
	}
	FStateEvent& OnExitState()
	{
		return StateExitEvent;
	}
	FStateEvent& OnFullyBlendState()
	{
		return StateFullyBlendEvent;
	}

protected:
	// Must be called from the AnimBP to notify the listeners about Enter State event.
	UFUNCTION(BlueprintCallable)
	void BroadcastOnEnterState(const FString& StateName);
	// Must be called from the AnimBP to notify the listeners about Left State event.
	UFUNCTION(BlueprintCallable)
	void BroadcastOnExitState(const FString& StateName);
	// Must be called from the AnimBP to notify the listeners about Fully-Blend State event.
	UFUNCTION(BlueprintCallable)
	void BroadcastOnFullyBlendState(const FString& StateName);

private:
	// Event Dispatchers
	FStateEvent StateEnterEvent;
	FStateEvent StateExitEvent;
	FStateEvent StateFullyBlendEvent;
};
