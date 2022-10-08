// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"

AHealingItem::AHealingItem()
{

}

void AHealingItem::BeginPlay()
{
    Super::BeginPlay();
}
bool AHealingItem::UseItem()
{
    Super::UseItem();
    UE_LOG(LogTemp, Display, TEXT("I'm a healing item"));
    return true;
}