// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include "GameCharacterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AGameCharacter::AGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	const auto TempMesh = GetMesh();
	if (IsValid(TempMesh))
	{
		AnimInstance = Cast<UGameCharacterAnimInstance>(TempMesh->GetAnimInstance());
		if (!IsValid(AnimInstance)) {
			// TODO: Error logs
		}
	}
}

// Called every frame
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimationProperties();
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGameCharacter::UpdateAnimationProperties()
{
	if (IsValid(AnimInstance))
	{
		const auto TempMoveComp = GetMovementComponent();
		if (IsValid(TempMoveComp))
		{
			AnimInstance->MoveSpeed = TempMoveComp->Velocity.Size();
			AnimInstance->bIsFalling = TempMoveComp->IsFalling();
		}
	}
}
