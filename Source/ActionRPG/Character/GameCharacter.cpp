// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include "GameCharacterAnimInstance.h"

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
	const auto MovementComp = GetCharacterMovement();
	if (AnimInstance.IsValid())
	{
		AnimInstance->MoveSpeed = MovementComp->Velocity.Size();
		AnimInstance->bIsFalling = MovementComp->IsFalling();
	}
}
