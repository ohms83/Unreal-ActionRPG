// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportAttackCommand.h"
#include "TeleportCommand.h"
#include "SetVisibilityCommand.h"
#include "ActionRPG/Component/AttackBehavior.h"

bool UTeleportAttackCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	return CommandQueue->Run(Executor, DeltaTime);
}

void UTeleportAttackCommand::OnBeginExecution(AActor* Executor)
{
	CommandQueue = NewObject<USequentialCommand>();

	UTeleportCommand* TeleportCommand = NewObject<UTeleportCommand>();
	TeleportCommand->TeleportTime = TeleportTime;
	TeleportCommand->TargetLocation = TargetLocation;
	TeleportCommand->VFX = VFX;
	TeleportCommand->SFX = SFX;

	USetVisibilityCommand* SetVisibilityCommand = NewObject<USetVisibilityCommand>();
	SetVisibilityCommand->bVisible = false;
	SetVisibilityCommand->Seconds = TeleportTime;

	UParallelCommand* ParallelQueue = NewObject<UParallelCommand>();
	ParallelQueue->Add(TeleportCommand);
	ParallelQueue->Add(SetVisibilityCommand);

	UOneshotCommand* AttackCommand = NewObject<UOneshotCommand>();
	AttackCommand->ExecutionDelegate.BindLambda([this](AActor* ExecuteActor) {
		UAttackBehavior* AttackBehavior =
			Cast<UAttackBehavior>(ExecuteActor->GetComponentByClass(UAttackBehavior::StaticClass()));

		if (IsValid(AttackBehavior))
		{
			AttackBehavior->RegisterAttackCommand(AttackData);
		}
	});

	CommandQueue->Add(ParallelQueue);
	CommandQueue->Add(AttackCommand);
}
