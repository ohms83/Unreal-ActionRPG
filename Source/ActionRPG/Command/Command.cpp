#include "Command.h"

bool UCommand::Run(AActor* Executor, float DeltaTime)
{
	if (bIsFinished || !IsValid(Executor)) {
		return true;
	}

	if (!bIsActive)
	{
		bIsActive = true;
		OnBeginExecution(Executor);
		OnBeginExecutionDelegate.Broadcast(Executor, this);
	}

	bIsFinished = RunInternal(Executor, DeltaTime);
	if (bIsFinished)
	{
		OnFinishExecution(Executor);
		OnFinishExecutionDelegate.Broadcast(Executor, this);
	}

	return bIsFinished;
}

bool UOneshotCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	ExecutionDelegate.Execute(Executor);
	return true;
}

bool UParallelCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	bool bIsFinishedAll = true;
	for (auto Command : CommandList)
	{
		if (Command.IsValid()) {
			bIsFinishedAll &= Command->Run(Executor, DeltaTime);
		}
	}
	return bIsFinishedAll;
}

void UParallelCommand::Add(UCommand* Command)
{
	CommandList.Add(Command);
}

bool USequentialCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	if (CommandQueue.Num() <= 0)
	{
		return true;
	}

	TWeakObjectPtr<UCommand> CurrentCommand = CommandQueue[0];
	if (!CurrentCommand.IsValid() || CurrentCommand->Run(Executor, DeltaTime))
	{
		CommandQueue.RemoveAt(0);
	}
	return CommandQueue.Num() <= 0;
}

void USequentialCommand::Add(UCommand* Command)
{
	CommandQueue.Add(Command);
}