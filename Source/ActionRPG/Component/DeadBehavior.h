// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeadBehavior.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UDeadBehavior : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDeadBehavior();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dead Behavior")
	void PlayDeadVFXAndAnimation(FDamageEvent const& DamageEvent, AActor* DamageCauser);

private: // Dead
	UPROPERTY(EditDefaultsOnly, Category = "Battle Character|Dead", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeadMontage;
		
};
