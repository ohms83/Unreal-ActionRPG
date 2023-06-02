// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatsFunctionLibrary.h"

float UCharacterStatsFunctionLibrary::GetHpPercent(const FCharacterStats& Stats)
{
	return Stats.GetHpPercent();
}

FCharacterStats UCharacterStatsFunctionLibrary::AddStats(const FCharacterStats& A, const FCharacterStats& B)
{
	return A + B;
}

FCharacterStats UCharacterStatsFunctionLibrary::MinusStats(const FCharacterStats& A, const FCharacterStats& B)
{
	return A - B;
}

FCharacterStats UCharacterStatsFunctionLibrary::MulStats(const FCharacterStats& Stats, float Multiplier)
{
	return Stats * Multiplier;
}

FCharacterStats UCharacterStatsFunctionLibrary::DivStats(const FCharacterStats& Stats, float Divider)
{
	return Stats / Divider;
}
