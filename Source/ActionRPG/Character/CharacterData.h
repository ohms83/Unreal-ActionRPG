// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FStatsValue
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 Value = 0;

	FStatsValue operator + (const FStatsValue& Rhs) const;
	FStatsValue& operator += (const FStatsValue& Rhs);

	FStatsValue operator - (const FStatsValue& Rhs) const;
	FStatsValue& operator -= (const FStatsValue& Rhs);

	bool operator == (const FStatsValue& Rhs) const;
	bool operator == (uint32 UintValue);
	bool operator != (const FStatsValue& Rhs) const;
	bool operator != (uint32 UintValue);
	bool operator < (const FStatsValue& Rhs) const;
	bool operator < (uint32 UintValue);
	bool operator > (const FStatsValue& Rhs) const;
	bool operator > (uint32 UintValue);
};

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint32 Hp;
	UPROPERTY(EditAnywhere)
	uint32 Mp;
	UPROPERTY(EditAnywhere)
	uint32 MaxHp;
	UPROPERTY(EditAnywhere)
	uint32 MaxMp;
	UPROPERTY(EditAnywhere)
	uint32 Atk;
	UPROPERTY(EditAnywhere)
	uint32 Def;
	UPROPERTY(EditAnywhere)
	uint32 MAtk;
	UPROPERTY(EditAnywhere)
	uint32 MDef;

	FCharacterStats operator + (const FCharacterStats& Rhs) const;
	FCharacterStats& operator += (const FCharacterStats& Rhs);

	FCharacterStats operator - (const FCharacterStats& Rhs) const;
	FCharacterStats& operator -= (const FCharacterStats& Rhs);
};
