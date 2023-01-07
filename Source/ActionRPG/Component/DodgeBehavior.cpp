// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeBehavior.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

bool UDodgeBehavior::Dodge(const FVector& Direction)
{
	if (!CanDodge())
	{
		return false;
	}

	// Turn the actor toward the dodge direction
	if (!Direction.IsZero())
	{
		auto TempOwner = GetOwner();
		const auto Location = TempOwner->GetActorLocation();
		const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + Direction);
		TempOwner->SetActorRotation(Rotator);
	}

	// Play dodge montage;
	float Seconds = AnimInstance->Montage_Play(
		DodgeMontage,
		AnimPlayRate,
		EMontagePlayReturnType::MontageLength,
		MontageStartAt);

	if (MontageStartAt >= Seconds) {
		UE_LOG(LogTemp, Warning,
			TEXT("MontageStartAt is greater or equal the length of animation montage itself. Is this intentional? %.2f:%.2f"),
			MontageStartAt, Seconds);
		return false;
	}
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

