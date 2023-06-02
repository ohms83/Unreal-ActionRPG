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
	int32 Hp;
	UPROPERTY(EditAnywhere)
	int32 Mp;
	UPROPERTY(EditAnywhere)
	int32 MaxHp;
	UPROPERTY(EditAnywhere)
	int32 MaxMp;
	UPROPERTY(EditAnywhere)
	int32 Atk;
	UPROPERTY(EditAnywhere)
	int32 Def;
	UPROPERTY(EditAnywhere)
	int32 MAtk;
	UPROPERTY(EditAnywhere)
	int32 MDef;

	FCharacterStats operator + (const FCharacterStats& Rhs) const;
	FCharacterStats& operator += (const FCharacterStats& Rhs);

	FCharacterStats operator - (const FCharacterStats& Rhs) const;
	FCharacterStats& operator -= (const FCharacterStats& Rhs);
};
