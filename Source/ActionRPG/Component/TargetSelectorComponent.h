// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActorComponent.h"
#include "TargetSelectorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UTargetSelectorComponent : public UGameActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetSelectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Select the most sutiable target based on the specified input axis
	UFUNCTION(BlueprintCallable, Category = "Target Selector")
	void SelectTargetFromInput(FVector2D InputAxis);
	UFUNCTION(BlueprintCallable, Category = "Target Selector")
	void SelectTarget(AActor* NextTarget);
	UFUNCTION(BlueprintCallable, Category = "Target Selector")
	void SelectNextTarget();
	UFUNCTION(BlueprintCallable, Category = "Target Selector")
	AActor* GetSelectedTarget() const { return Target.Get(); }

	// Get the actor that's currently targetting its owner
	UFUNCTION(BlueprintCallable, Category = "Target Selector")
	AActor* GetActorTargettingOwner() const { return BeingTargettedBy.Get(); }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Selector")
	bool bSmoothRotation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Selector")
	float RotationRate = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Selector")
	float TargetSearchRadius = 1000.f;

protected:
	void OnTargetDead(class ABattleCharacter* Target);

	void SelectTarget_Internal(AActor* NextTarget);
	void UnselectTarget_Internal(AActor* NextTarget);

	void UpdatetargetList();
	void RemoveFromTargetList(AActor* Actor);

	static void SetBeingTargettedBy(AActor* TargettingActor, AActor* TargetActor);

private:
	TWeakObjectPtr<AActor> Target;
	// The actor that's currently targetting its owner
	TWeakObjectPtr<AActor> BeingTargettedBy;
	FDelegateHandle OnTargetDeadDelegateHandle;
	TArray<TWeakObjectPtr<AActor>> TargetList;
};
