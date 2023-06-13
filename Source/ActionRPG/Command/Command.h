// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Command.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FCommandDelegate, AActor*, class UCommand*)

UCLASS(Blueprintable)
class ACTIONRPG_API UCommand : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Command")
	bool Run(AActor* Executor, float DeltaTime);

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Command", meta = (DisplayName = "OnRun"))
	bool RunInternal(AActor* Executor, float DeltaTime);
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) { return true; }

	virtual void OnBeginExecution(AActor* Executor) {}
	virtual void OnFinishExecution(AActor* Executor) {}

public:
	FCommandDelegate OnBeginExecutionDelegate;
	FCommandDelegate OnFinishExecutionDelegate;

private:
	bool bIsActive = false;
	bool bIsFinished = false;
};

// A type of command that immediately terminate after executing once.
UCLASS()
class ACTIONRPG_API UOneshotCommand : public UCommand
{
	GENERATED_BODY()
public:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;

public:
	// Command execution delegate.
	TDelegate<void(AActor*)> ExecutionDelegate;
};

// A collection of commands that will run in sequential order
// The command is considered finished when all the sub-commands finished execution.
UCLASS()
class ACTIONRPG_API USequentialCommand : public UCommand
{
	GENERATED_BODY()
public:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Command|Sequential")
	void Add(UCommand* Command);

private:
	TArray<TWeakObjectPtr<UCommand>> CommandQueue;
};

// A collection of commands that will be run in parallel.
// The command is considered finished when all the sub-commands finished execution.
UCLASS()
class ACTIONRPG_API UParallelCommand : public UCommand
{
	GENERATED_BODY()
public:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Command|Parallel")
	void Add(UCommand* Command);
private:
	// List of commands to Run
	TArray< TWeakObjectPtr<UCommand>> CommandList;
};