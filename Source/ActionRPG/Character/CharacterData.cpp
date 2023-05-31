// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterData.h"

FStatsValue FStatsValue::operator+(const FStatsValue& Rhs) const
{
	int64 NewValue = Value + Rhs.Value;
	FStatsValue Result = { (NewValue > UINT32_MAX ? UINT32_MAX : (uint32)NewValue) };
	return Result;
}

FStatsValue& FStatsValue::operator+=(const FStatsValue& Rhs)
{
	int64 NewValue = Value + Rhs.Value;
	Value = (NewValue > UINT32_MAX ? UINT32_MAX : (uint32)NewValue);
	return *this;
}

FStatsValue FStatsValue::operator-(const FStatsValue& Rhs) const
{
	int64 NewValue = (int64)Value - (int64)Rhs.Value;
	FStatsValue Result = { (NewValue < 0 ? 0 : (uint32)NewValue) };
	return Result;
}

FStatsValue& FStatsValue::operator-=(const FStatsValue& Rhs)
{
	int64 NewValue = (int64)Value - (int64)Rhs.Value;
	Value = (NewValue < 0 ? 0 : (uint32)NewValue);
	return *this;
}

bool FStatsValue::operator==(const FStatsValue& Rhs) const
{
	return Value == Rhs.Value;
}

bool FStatsValue::operator==(uint32 UintValue)
{
	return Value == UintValue;
}

bool FStatsValue::operator!=(const FStatsValue& Rhs) const
{
	return Value != Rhs.Value;
}

bool FStatsValue::operator!=(uint32 UintValue)
{
	return Value != UintValue;
}

bool FStatsValue::operator<(const FStatsValue& Rhs) const
{
	return Value < Rhs.Value;
}

bool FStatsValue::operator<(uint32 UintValue)
{
	return Value < UintValue;
}

bool FStatsValue::operator>(const FStatsValue& Rhs) const
{
	return Value > Rhs.Value;
}

bool FStatsValue::operator>(uint32 UintValue)
{
	return Value > UintValue;
}

FCharacterStats FCharacterStats::operator+(const FCharacterStats& Rhs) const
{
	FCharacterStats Result{0};

	Result.Hp = Hp + Rhs.Hp;
	Result.Mp = Mp + Rhs.Mp;
	Result.MaxHp = MaxHp + Rhs.MaxHp;
	Result.MaxMp = MaxMp + Rhs.MaxMp;
	Result.Atk = Atk + Rhs.Atk;
	Result.Def = Def + Rhs.Def;
	Result.MAtk = Def + Rhs.MAtk;
	Result.MDef = MDef + Rhs.MDef;

	return Result;
}

FCharacterStats& FCharacterStats::operator+=(const FCharacterStats& Rhs)
{
	Hp += Rhs.Hp;
	Mp += Rhs.Mp;
	MaxHp += Rhs.MaxHp;
	MaxMp += Rhs.MaxMp;
	Atk += Rhs.Atk;
	Def += Rhs.Def;
	MAtk += Rhs.MAtk;
	MDef += Rhs.MDef;

	return *this;
}

FCharacterStats FCharacterStats::operator-(const FCharacterStats& Rhs) const
{
	FCharacterStats Result{ 0 };

	Result.Hp = Hp - Rhs.Hp;
	Result.Mp = Mp - Rhs.Mp;
	Result.MaxHp = MaxHp - Rhs.MaxHp;
	Result.MaxMp = MaxMp - Rhs.MaxMp;
	Result.Atk = Atk - Rhs.Atk;
	Result.Def = Def - Rhs.Def;
	Result.MAtk = Def - Rhs.MAtk;
	Result.MDef = MDef - Rhs.MDef;

	return Result;
}

FCharacterStats& FCharacterStats::operator-=(const FCharacterStats& Rhs)
{
	Hp -= Rhs.Hp;
	Mp -= Rhs.Mp;
	MaxHp -= Rhs.MaxHp;
	MaxMp -= Rhs.MaxMp;
	Atk -= Rhs.Atk;
	Def -= Rhs.Def;
	MAtk -= Rhs.MAtk;
	MDef -= Rhs.MDef;

	return *this;
}
