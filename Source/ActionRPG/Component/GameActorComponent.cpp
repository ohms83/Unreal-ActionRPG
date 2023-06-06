// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActorComponent.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UGameActorComponent::UGameActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool UGameActorComponent::TempLock(const UObject* Locker, float Seconds)
{
	if (LockComponent(Locker))
	{
		GetWorld()->GetTimerManager().SetTimer(
			TempLockTimerHandle,
			this,
			&UGameActorComponent::ReleaseTempLock,
			Seconds,
			false
		);

		return true;
	}
	return false;
}

// Called when the game starts
void UGameActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	const auto OwnerPawn = Cast<ACharacter>(GetOwner());
	if (IsValid(OwnerPawn))
	{
		_MovementComp = OwnerPawn->GetCharacterMovement();

		const auto TempMesh = OwnerPawn->GetMesh();
		if (IsValid(TempMesh))
		{
			_AnimInstance = Cast<UAnimInstance>(TempMesh->GetAnimInstance());
		}
	}
}

void UGameActorComponent::ReleaseTempLock()
{
	UnlockComponent(ComponentLocker.Get());
}

// Called every frame
void UGameActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TWeakObjectPtr<UAnimInstance> UGameActorComponent::GetAnimInstance() const
{
	return MakeWeakObjectPtr(_AnimInstance);
}

TWeakObjectPtr<UCharacterMovementComponent> UGameActorComponent::GetMovementComponent() const
{
	return MakeWeakObjectPtr(_MovementComp);
}

bool UGameActorComponent::LockComponent(const UObject* Locker)
{
	if (ComponentLocker.IsValid() && ComponentLocker != Locker)
	{
		return false;
	}
	ComponentLocker = Locker;
	Lock_Internal();
	return true;
}

void UGameActorComponent::UnlockComponent(const UObject* Locker)
{
	if (ComponentLocker.IsValid() && ComponentLocker == Locker)
	{
		ComponentLocker.Reset();
		TempLockTimerHandle.Invalidate();
	}
}
