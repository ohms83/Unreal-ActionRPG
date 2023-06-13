// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include "GameCharacterAnimInstance.h"

// Sets default values
AGameCharacter::AGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGameCharacter::RegisterCommand(UCommand* Command)
{
	if (Implements<UCommandExecutor>())
	{
		ICommandExecutor::Execute_QueueCommand(this, Command);
	}
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

	if (Implements<UCommandExecutor>())
	{
		ICommandExecutor::Execute_RunCommand(this, DeltaTime);
	}
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGameCharacter::SetTimer(FTimerHandle& OutHandle, FTimerDelegate Delegate, float Rate, bool bLoop, float FirstDelay)
{
	GetWorld()->GetTimerManager().SetTimer(
		OutHandle,
		Delegate,
		Rate,
		bLoop,
		FirstDelay
	);
}

void AGameCharacter::ClearTimer(FTimerHandle& OutHandle)
{
	GetWorld()->GetTimerManager().ClearTimer(OutHandle);
}

bool AGameCharacter::IsTimerActive(FTimerHandle TimerHandle)
{
	return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle);
}
