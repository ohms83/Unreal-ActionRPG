// Fill out your copyright notice in the Description page of Project Settings.


#include "SetVisibilityCommand.h"
#include "GameFramework/Character.h"

bool USetVisibilityCommand::RunInternal_Implementation(AActor* Executor, float DeltaTime)
{
	Countdown -= DeltaTime;
	return Countdown <= 0;
}

void USetVisibilityCommand::OnBeginExecution(AActor* Executor)
{
	ACharacter* Character = Cast<ACharacter>(Executor);
	Mesh = IsValid(Character) ?
		Character->GetMesh() :
		Cast<UMeshComponent>(Executor->GetComponentByClass(UMeshComponent::StaticClass()));

	if (Mesh.IsValid())
	{
		bPrevVisibility = Seconds < 0 ? bVisible : Mesh->GetVisibleFlag();
		Mesh->SetVisibility(bVisible);
	}

	Countdown = Seconds;
}

void USetVisibilityCommand::OnFinishExecution(AActor* Executor)
{
	if (Mesh.IsValid()) {
		Mesh->SetVisibility(bPrevVisibility);
	}
}
