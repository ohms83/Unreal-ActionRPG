// Fill out your copyright notice in the Description page of Project Settings.


#include "DelayCommand.h"
#include "DelayCommand.h"

bool UDelayCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	Countdown -= DeltaTime;
	return Countdown <= 0;
}

void UDelayCommand::OnBeginExecution(AActor* Executor)
{
	Countdown = DelayTime;
}
