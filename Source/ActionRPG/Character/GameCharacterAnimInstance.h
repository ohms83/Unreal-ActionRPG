// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameCharacterAnimInstance.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Properties")
	float MoveSpeed = 0;
	// Flag indicating whether the character is falling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Properties")
	bool bIsFalling;
	// Axis value from the input event.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Properties")
	FVector2D InputAxis;
};
