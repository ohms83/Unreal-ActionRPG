// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleData.generated.h"

#define MELEE_DAMAGE_EVENT_CLASS_ID 1000

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimPlayRate = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageModifier = 1.0f;
	// Hit-stop in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitStop = 0.0f;
	// Time dilation durint hit-stop
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitStopTimeDilation = 0.1f;
	// Knockback velocity (relative to the attacker)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D KnockbackDirection = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackSpeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator VFXOrientation;
};

USTRUCT(BlueprintType)
struct FMeleeDamageEvent : public FPointDamageEvent
{
	GENERATED_BODY()

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = MELEE_DAMAGE_EVENT_CLASS_ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackData AttackData;

	virtual int32 GetTypeID() const override { return FMeleeDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override;
};
