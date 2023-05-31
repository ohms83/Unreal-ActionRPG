// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionRPG/Character/CharacterData.h"

#include "DrawDebugHelpers.h"

#include "Equipment.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None		UMETA(DisplayName = "None"),
	Weapon		UMETA(DisplayName = "Weapon"),
	Shield		UMETA(DisplayName = "Shield"),
	Armor		UMETA(DisplayName = "Armor"),
	Accessory	UMETA(DisplayName = "Accessory"),
};

UCLASS()
class ACTIONRPG_API AEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	const FCharacterStats& GetStats() const { return Stats; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetStats(const FCharacterStats& NewStats) { Stats = NewStats; }

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	EEquipmentType GetEquipType() const { return Type; }
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipType(EEquipmentType NewType) { Type = NewType; }

public:
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
	class ACharacter* OwnerChar = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FCharacterStats Stats;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	EEquipmentType Type;
};

USTRUCT(BlueprintType)
struct FWeaponCollider
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Offset;
	UPROPERTY(EditAnywhere)
	FVector HalfSize;
	UPROPERTY(EditAnywhere)
	FRotator Orientation;
};

USTRUCT(BlueprintType)
struct FHitVFX
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ParticleTemplate;
	UPROPERTY(EditAnywhere)
	FVector Offset;
	UPROPERTY(EditAnywhere)
	FRotator Orientation;
	UPROPERTY(EditAnywhere)
	FVector Scale;
};

UCLASS()
class ACTIONRPG_API AWeapon : public AEquipment
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public: // Collision
	void EnableTraceHit(bool bEnable);

	void SetDebugDrawEnable(bool bEnable);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FTraceHitDelegate, AWeapon* /* ThisWeapon */, const TArray<FHitResult>& /* HitResults */);
	FTraceHitDelegate OnTraceHit;

private: // Collision
	void GetBoxTransform(FVector& OutLocation, FRotator& OutOrientation);

private: // Collision
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FWeaponCollider BoxCollision;

	bool bTraceHitEnabled = false;
	FVector BoxPrevLocation;

#if ENABLE_DRAW_DEBUG
	bool bEnableDebugDraw = false;
#endif

public: // VFX
	const FHitVFX& GetHitVFX() const;

private: // VFX
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FHitVFX HitVFX;
};
