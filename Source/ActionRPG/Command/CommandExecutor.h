// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Command.h"
#include "CommandExecutor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCommandExecutor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API ICommandExecutor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Command Executor")
	void RunCommand(float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Command Executor")
	void QueueCommand(UCommand* NextCommand);

protected:
	virtual void RunCommand_Implementation(float DeltaTime);
	virtual void QueueCommand_Implementation(UCommand* NextCommand);
private:
	TArray<TWeakObjectPtr<UCommand>> CommandQueue;
};
