// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"
#include "SetVisibilityCommand.generated.h"

/**
 * Change Mesh's visibility for the specified time.
 */
UCLASS()
class ACTIONRPG_API USetVisibilityCommand : public UCommand
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Set Visibility")
	bool bVisible = false;
	// How many seconds the new visibility setting should be intact.
	// The default value -1 means the visibility won't be reset to the old value
	// and the command will terminate immediately
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command|Set Visibility")
	float Seconds = -1;

protected:
	virtual bool RunInternal_Implementation(AActor* Executor, float DeltaTime) override;
	virtual void OnBeginExecution(AActor* Executor) override;
	virtual void OnFinishExecution(AActor* Executor) override;

private:
	TWeakObjectPtr<UMeshComponent> Mesh;
	bool bPrevVisibility = false;
	float Countdown = 0;
};
