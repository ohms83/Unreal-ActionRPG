// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSelectorComponent.h"

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
}

void UTargetSelectorComponent::SelectTargetFromInput(FVector2D InputAxis)
{
	// TODO: ...
}

void UTargetSelectorComponent::SelectTarget(AActor* NextTarget)
{
	if (IsLocked()) {
		return;
	}

	SetComponentTickEnabled(IsValid(NextTarget));

	/*APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AThirdPersonController* OwnerController = Cast<AThirdPersonController>(OwnerPawn->GetController());
	if (IsValid(OwnerController))
	{
		OwnerController->SetStrafeWalk(bValidTarget);
	}*/

	if (IsValid(NextTarget))
	{
		SelectTarget_Internal(NextTarget);
	}
	else if(Target.IsValid())
	{
		UnselectTarget_Internal(Target.Get());
	}

	Target = NextTarget;
}

void UTargetSelectorComponent::OnTargetDead(ABattleCharacter* DeadTarget)
{
	if (IsValid(DeadTarget) && Target == DeadTarget)
	{
		// Unselect the target
		SelectTarget(nullptr);
	}
}

void UTargetSelectorComponent::SelectTarget_Internal(AActor* NextTarget)
{
	ABattleCharacter* NextTargetChar = Cast<ABattleCharacter>(NextTarget);

	if (!NextTargetChar->IsDead())
	{
		NextTargetChar->ShowCharacterOutline(ECharacterOutlineType::Enemy);
		OnTargetDeadDelegateHandle =
			NextTargetChar->OnDeadDelegate.AddUObject(this, &UTargetSelectorComponent::OnTargetDead);
	}
	else
	{
		SetComponentTickEnabled(false);
	}

	SetBeingTargettedBy(this->GetOwner(), NextTarget);
}

void UTargetSelectorComponent::UnselectTarget_Internal(AActor* NextTarget)
{
	ABattleCharacter* CurrentTargetChar = Cast<ABattleCharacter>(NextTarget);
	CurrentTargetChar->ShowCharacterOutline(ECharacterOutlineType::None);
	CurrentTargetChar->OnDeadDelegate.Remove(OnTargetDeadDelegateHandle);

	SetBeingTargettedBy(nullptr, Target.Get());
}

void UTargetSelectorComponent::SetBeingTargettedBy(AActor* TargettingActor, AActor* TargetActor)
{
	if (!IsValid(TargetActor)) {
		return;
	}

	UTargetSelectorComponent* TargetSelectorComponent =
		Cast<UTargetSelectorComponent>(TargetActor->GetComponentByClass(UTargetSelectorComponent::StaticClass()));

	if (IsValid(TargetSelectorComponent))
	{
		TargetSelectorComponent->BeingTargettedBy = TargettingActor;
	}
}
