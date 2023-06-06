// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "GameActorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UGameActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGameActorComponent();

	/**
	* Lock this component from executing any commands. If the component is already locked by
	* the other object, the function return false.
	* @param Locker A pointer to the object that's calling this function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Actor Component")
	bool LockComponent(const UObject* Locker);

	/**
	* Unlock this component from executing any commands. If the component wasn't locked by
	* the given Locker, the function silently returns.
	* @param Locker A pointer to the object that's calling this function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Actor Component")
	void UnlockComponent(const UObject* Locker);

	UFUNCTION(BlueprintCallable, Category = "Game Actor Component")
	bool IsLocked() const { return ComponentLocker.IsValid(); }

	// Temporary locking the component for the specified seconds.
	UFUNCTION(BlueprintCallable, Category = "Game Actor Component")
	bool TempLock(const UObject* Locker, float Seconds);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Child classes should override this to implement their own logics
	virtual void Lock_Internal() {}

	void ReleaseTempLock();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	TWeakObjectPtr<UAnimInstance> GetAnimInstance() const;
	TWeakObjectPtr<UCharacterMovementComponent> GetMovementComponent() const;

private:
	// An object that acquire a lock to this component.
	TWeakObjectPtr<const UObject> ComponentLocker;
	FTimerHandle TempLockTimerHandle;

	UAnimInstance* _AnimInstance;
	UCharacterMovementComponent* _MovementComp;
};
