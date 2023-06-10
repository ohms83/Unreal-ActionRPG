// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameCharacter.generated.h"

class UGameCharacterAnimInstance;
class UCharacterMovementComponent;

UCLASS()
class ACTIONRPG_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected: // Timer
	void SetTimer(FTimerHandle& OutHandle, FTimerDelegate Delegate, float Rate, bool bLoop, float FirstDelay = -1.0f);
	void ClearTimer(FTimerHandle& OutHandle);
	bool IsTimerActive(FTimerHandle TimerHandle);

protected:
	TWeakObjectPtr<UGameCharacterAnimInstance> AnimInstance;
};
