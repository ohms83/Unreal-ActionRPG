// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleData.h"

bool FMeleeDamageEvent::IsOfType(int32 InID) const
{
    return (FMeleeDamageEvent::ClassID == InID) ||
        FPointDamageEvent::IsOfType(InID) ||
        FDamageEvent::IsOfType(InID);
}
