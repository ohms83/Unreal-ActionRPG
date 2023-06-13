// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"
#include "DelayCommand.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UDelayCommand : public UCommand
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Delay")
	float DelayTime = 0;

protected:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;
	virtual void OnBeginExecution(AActor* Executor) override;

private:
	float Countdown = 0;
};
