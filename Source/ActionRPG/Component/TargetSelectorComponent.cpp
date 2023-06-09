// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSelectorComponent.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

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

	// The following section ONLY works when the Target is valid
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

	const bool bValidTarget = IsValid(NextTarget);
	SetComponentTickEnabled(bValidTarget);

	/*APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AThirdPersonController* OwnerController = Cast<AThirdPersonController>(OwnerPawn->GetController());
	if (IsValid(OwnerController))
	{
		OwnerController->SetStrafeWalk(bValidTarget);
	}*/

	if (bValidTarget)
	{
		SelectTarget_Internal(NextTarget);
	}
	if(Target.IsValid())
	{
		UnselectTarget_Internal(Target.Get());
	}

	Target = NextTarget;
}

void UTargetSelectorComponent::SelectNextTarget()
{
	UpdatetargetList();

	if (TargetList.Num() == 0) {
		return;
	}

	int32 Index = (Target.IsValid() ? TargetList.Find(Target) : -1) + 1;
	if (TargetList.IsValidIndex(Index)) {
		SelectTarget(TargetList[Index].Get());
	}
	else {
		SelectTarget(nullptr);
	}
}

void UTargetSelectorComponent::OnTargetDead(ABattleCharacter* DeadTarget)
{
	if (IsValid(DeadTarget) && Target == DeadTarget)
	{
		// Unselect the target
		SelectTarget(nullptr);
		RemoveFromTargetList(DeadTarget);
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

void UTargetSelectorComponent::UpdatetargetList()
{
	// Remove invalidated actors
	TArray<TWeakObjectPtr<AActor>> ActorsToRemove;
	for (const auto ActorPtr : TargetList)
	{
		if (!ActorPtr.IsValid()) {
			ActorsToRemove.AddUnique(ActorPtr);
		}
	}

	for (const auto ActorPtr : ActorsToRemove) {
		TargetList.Remove(ActorPtr);
	}

	// Searches for new targets
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn),
	};
	TArray<AActor*> ActorsToIgnore = {
		GetOwner(),
	};

	TArray<AActor*> Results;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetOwner()->GetActorLocation(),
		TargetSearchRadius,
		ObjectTypes,
		ABattleCharacter::StaticClass(),
		ActorsToIgnore,
		Results
	);

	FVector OwnerLocation = GetOwner()->GetActorLocation();
	Results.Sort([OwnerLocation](const AActor& A, const AActor& B) -> bool
		{
			float DA = FVector::Dist(OwnerLocation, A.GetActorLocation());
			float DB = FVector::Dist(OwnerLocation, B.GetActorLocation());
			return DA < DB;
		}
	);

	for (auto Actor : Results)
	{
		TargetList.AddUnique(Actor);
	}
}

void UTargetSelectorComponent::RemoveFromTargetList(AActor* Actor)
{
	TWeakObjectPtr<AActor> ActorToRemove;
	for (const auto ActorPtr : TargetList)
	{
		if (ActorPtr.Get() == Actor) {
			ActorToRemove = ActorPtr;
			break;
		}
	}

	TargetList.Remove(ActorToRemove);
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
