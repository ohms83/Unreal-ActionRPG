// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeBehavior.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UDodgeBehavior::UDodgeBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDodgeBehavior::BeginPlay()
{
	Super::BeginPlay();

	// ...
	const auto OwnerPawn = Cast<ACharacter>(GetOwner());
	if (IsValid(OwnerPawn))
	{
		MovementComp = Cast<UPawnMovementComponent>(OwnerPawn->GetComponentByClass(UPawnMovementComponent::StaticClass()));

		const auto TempMesh = OwnerPawn->GetMesh();
		if (IsValid(TempMesh))
		{
			AnimInstance = Cast<UAnimInstance>(TempMesh->GetAnimInstance());
		}
	}
}


// Called every frame
void UDodgeBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDodgeBehavior::Dodge()
{
	if (!CanDodge())
	{
		return false;
	}
	// Play dodge montage;
	float Seconds = AnimInstance->Montage_Play(DodgeMontage);
	return Seconds > 0;
}

bool UDodgeBehavior::IsDodging() const
{
	if (!IsValid(DodgeMontage) || !IsValid(AnimInstance)) {
		return false;
	}

	return AnimInstance->Montage_IsPlaying(DodgeMontage);
}

bool UDodgeBehavior::CanDodge() const
{
	const bool bIsDodging = IsDodging();
	const bool bIsFalling = !IsValid(MovementComp) || MovementComp->IsFalling();
	return !bIsDodging && !bIsFalling;
}

