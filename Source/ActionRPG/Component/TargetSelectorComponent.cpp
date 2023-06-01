// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSelectorComponent.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

#include "ActionRPG/Controller/ThirdPersonController.h"
#include "ActionRPG/Character/BattleCharacter.h"

// Sets default values for this component's properties
UTargetSelectorComponent::UTargetSelectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetSelectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// The Tick function will be disabled by default until a target has been set.
	SetComponentTickEnabled(false);
}


// Called every frame
void UTargetSelectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsLocked()) {
		return;
	}

	if (!Target.IsValid())
	{
		// Handles the case when the target is destroyed after it was set.
		SetComponentTickEnabled(false);
		return;
	}

	// ...
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn)) {
		return;
	}

	FVector TargetDirection = Target->GetActorLocation() - Pawn->GetActorLocation();
	TargetDirection.Z = 0; // We are not interested in the Z axis.

	FRotator Orientation = TargetDirection.Rotation();
	if (bSmoothRotation)
	{
		float Alpha = RotationRate == 0 ? 0 : DeltaTime / RotationRate;
		Orientation = FMath::Lerp(Pawn->GetActorRotation(), Orientation, Alpha);
	}
	Pawn->SetActorRotation(Orientation);

	ACharacter* TargetCharacter = Cast<ACharacter>(Target.Get());
	if (TargetCharacter)
	{
		FVector TargetCenter = TargetCharacter->GetActorLocation();
		TargetCenter.Z = TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
}

void UTargetSelectorComponent::SelectTargetFromInput(FVector2D InputAxis)
{
	// TODO: ...
}

void UTargetSelectorComponent::SelectTarget(AActor* NextTarget)
{
	const bool bValidTarget = IsValid(NextTarget);
	if (bValidTarget) {
		SetComponentTickEnabled(true);
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AThirdPersonController* Controller = Cast<AThirdPersonController>(OwnerPawn->GetController());

	if (IsValid(Controller))
	{
		//Controller->SetStrafeWalk(bValidTarget);
	}

	ABattleCharacter* NextTargetChar = Cast<ABattleCharacter>(NextTarget);
	ABattleCharacter* CurrentTargetChar = Cast<ABattleCharacter>(Target);

	if (IsValid(NextTargetChar))
	{
		NextTargetChar->ShowCharacterOutline(ECharacterOutlineType::Enemy);
	}
	else if (IsValid(CurrentTargetChar))
	{
		CurrentTargetChar->ShowCharacterOutline(ECharacterOutlineType::None);
	}

	Target = NextTarget;
}
