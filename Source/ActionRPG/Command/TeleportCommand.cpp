// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportCommand.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

bool UTeleportCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	CountDown -= DeltaTime;
	return CountDown <= 0;
}

void UTeleportCommand::OnBeginExecution(AActor* Executor)
{
	CountDown = TeleportTime;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		Executor,
		VFX,
		Executor->GetActorLocation(),
		Executor->GetActorRotation(),
		FVector::OneVector
	);
}

void UTeleportCommand::OnFinishExecution(AActor* Executor)
{
	Executor->SetActorLocation(TargetLocation);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		Executor,
		VFX,
		TargetLocation,
		Executor->GetActorRotation(),
		FVector::OneVector
	);
}
