// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"
#include "TeleportCommand.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UTeleportCommand : public UCommand
{
	GENERATED_BODY()
protected:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;
	virtual void OnBeginExecution(AActor* Executor) override;
	virtual void OnFinishExecution(AActor* Executor) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport")
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport")
	float TeleportTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport")
	class UNiagaraSystem* VFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport")
	class USoundBase* SFX;

private:
	float CountDown = 0;
};
