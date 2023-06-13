// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandExecutor.h"

// Add default functionality here for any IExecutor functions that are not pure virtual.

void ICommandExecutor::RunCommand_Implementation(float DeltaTime)
{
	if (CommandQueue.Num() <= 0)
	{
		return;
	}

	TWeakObjectPtr<UCommand> CurrentCommand = CommandQueue[0];
	if (!CurrentCommand.IsValid() || CurrentCommand->Run(Cast<AActor>(this), DeltaTime))
	{
		CommandQueue.RemoveAt(0);
	}
}

void ICommandExecutor::QueueCommand_Implementation(UCommand* NextCommand)
{
	if (!IsValid(NextCommand)) {
		UE_LOG(LogTemp, Warning, TEXT("Cant' add an invalid UCommand object!"));
	}
	CommandQueue.Add(NextCommand);
}
