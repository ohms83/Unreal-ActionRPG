// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"
#include "ActionRPG/Battle/BattleData.h"
#include "TeleportAttackCommand.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UTeleportAttackCommand : public UCommand
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport Attack")
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport Attack")
	float TeleportTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport Attack")
	class UNiagaraSystem* VFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport Attack")
	class USoundBase* SFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Teleport Attack")
	FAttackData AttackData;

protected:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;
	virtual void OnBeginExecution(AActor* Executor) override;

private:
	UPROPERTY()
	USequentialCommand* CommandQueue;
};
