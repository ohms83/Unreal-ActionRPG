// Fill out your copyright notice in the Description page of Project Settings.


#include "CirclingAroundComponent.h"

// Sets default values for this component's properties
UCirclingAroundComponent::UCirclingAroundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCirclingAroundComponent::CircleAround(const AActor* Target, ECirclingDirection Direction)
{
	AnchorTarget = Target;
	CirclingDirection = Direction;
}

// Called when the game starts
void UCirclingAroundComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCirclingAroundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!IsMoving()) {
		return;
	}

	AActor* MyOwner = GetOwner();
	const FVector Location = MyOwner->GetActorLocation();
	const FVector TargetVector = AnchorTarget->GetActorLocation() - Location;
	const FVector LookVector = TargetVector.GetSafeNormal();
	const FRotator LookRotation = LookVector.Rotation();
	MoveDirection = LookRotation.RotateVector(FVector::RightVector) *
		(CirclingDirection == ECirclingDirection::Left ? -1 : 1);

	const FVector NewLocation = Location + (MoveDirection * MoveSpeed * DeltaTime);
	MyOwner->SetActorLocation(NewLocation, true);
	MyOwner->SetActorRotation(LookRotation);

	/*DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + (MoveVector * 100.f),
		50.f,
		FColor::Red
	);*/
}

