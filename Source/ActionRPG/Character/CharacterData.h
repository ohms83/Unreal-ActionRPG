// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 Hp = 0;
	UPROPERTY()
	uint32 Mp = 0;
	UPROPERTY()
	uint32 MaxHP = 0;
	UPROPERTY()
	uint32 MaxMP = 0;
	UPROPERTY()
	uint32 Atk = 0;
	UPROPERTY()
	uint32 Def = 0;
	UPROPERTY()
	uint32 MAtk = 0;
	UPROPERTY()
	uint32 MDef = 0;
};
