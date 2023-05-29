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
	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool Dodge(const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool IsDodging() const;
	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool CanDodge() const;

	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	UAnimMontage* GetDodgeMontage() const { return DodgeMontage; }

	/**
	* Lock this component from executing any commands. If the component is already locked by
	* the other object, the function return false.
	* @param Locker A pointer to the object that's calling this function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool LockComponent(const UObject* Locker);

	/**
	* Unlock this component from executing any commands. If the component wasn't locked by
	* the given Locker, the function silently returns.
	* @param Locker A pointer to the object that's calling this function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	void UnlockComponent(const UObject* Locker);

	UFUNCTION(BlueprintCallable, Category = "Dodge Behavior")
	bool IsLocked() const { return ComponentLocker.IsValid(); }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Dodge
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float AnimPlayRate = 1.0f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float MontageStartAt = 0.0f;

private:
	UAnimInstance* AnimInstance = nullptr;
	UPawnMovementComponent* MovementComp = nullptr;

	// An object that acquire a lock to this component.
	TWeakObjectPtr<const UObject> ComponentLocker;
};
