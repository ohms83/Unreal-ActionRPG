// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionRPG/Character/CharacterData.h"

#include "CharacterStatsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UCharacterStatsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Stats")
	static float GetHpPercent(const FCharacterStats& Stats);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Stats")
	static FCharacterStats AddStats(const FCharacterStats& A, const FCharacterStats& B);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Stats")
	static FCharacterStats MinusStats(const FCharacterStats& A, const FCharacterStats& B);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Stats")
	static FCharacterStats MulStats(const FCharacterStats& Stats, float Multiplier);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Stats")
	static FCharacterStats DivStats(const FCharacterStats& Stats, float Divider);
};
