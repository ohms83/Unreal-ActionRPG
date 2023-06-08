// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CirclingAroundComponent.generated.h"

UENUM(BlueprintType)
enum class ECirclingDirection : uint8
{
	None  UMETA(DisplayName = "None"),
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UCirclingAroundComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCirclingAroundComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Circling Around Component")
	void CircleAround(const AActor* Anchor, ECirclingDirection Direction);
	UFUNCTION(BlueprintCallable, Category = "Circling Around Component")
	const FVector& GetMoveDirection() const { return MoveDirection;  }
	UFUNCTION(BlueprintCallable, Category = "Circling Around Component")
	bool IsMoving() const { return IsValid(AnchorTarget); }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 200.f;

private:
	ECirclingDirection CirclingDirection;
	UPROPERTY()
	const AActor* AnchorTarget = nullptr;
	FVector MoveDirection;
};
