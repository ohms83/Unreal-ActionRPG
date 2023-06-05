// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadBehavior.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UDeadBehavior::UDeadBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDeadBehavior::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDeadBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeadBehavior::PlayDeadVFXAndAnimation(FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!IsValid(OwnerChar)) {
		return;
	}

	const auto TempMesh = OwnerChar->GetMesh();
	if (!IsValid(TempMesh))
	{
		return;
	}

	UAnimInstance* AnimInstance = TempMesh->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Stop(0.1f, nullptr);
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
	}
}

