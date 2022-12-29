// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 HP = 0;
	UPROPERTY()
	uint32 MP = 0;
	UPROPERTY()
	uint32 MaxHP = 0;
	UPROPERTY()
	uint32 MaxMP = 0;
	UPROPERTY()
	uint32 ATK = 0;
	UPROPERTY()
	uint32 DEF = 0;
	UPROPERTY()
	uint32 MATK = 0;
	UPROPERTY()
	uint32 MDEF = 0;
};
