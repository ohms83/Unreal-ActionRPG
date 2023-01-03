// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DodgeBehavior.generated.h"

class UAnimInstance;
class UPawnMovementComponent;

// An ActorComponent responsibles for implementing Character Dodging's logic
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UDodgeBehavior : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDodgeBehavior();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Try executing dodge animation. Return true if succeeded.
	UFUNCTION(BlueprintCallable, Category = "Character Behavior")
	bool Dodge();

	UFUNCTION(BlueprintCallable, Category = "Character Behavior")
	bool IsDodging() const;
	UFUNCTION(BlueprintCallable, Category = "Character Behavior")
	bool CanDodge() const;

	UFUNCTION(BlueprintCallable, Category = "Character Behavior")
	UAnimMontage* GetDodgeMontage() const { return DodgeMontage; }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Dodge
	UPROPERTY(EditDefaultsOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage = nullptr;

private:
	UAnimInstance* AnimInstance = nullptr;
	UPawnMovementComponent* MovementComp = nullptr;
};
