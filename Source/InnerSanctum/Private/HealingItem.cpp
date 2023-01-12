// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "HealthComponent.h"
#include "ProtagonistCharacter.h"

AHealingItem::AHealingItem()
{

}

void AHealingItem::BeginPlay()
{
    Super::BeginPlay();
}
bool AHealingItem::UseItem()
{
    UHealthComponent* healthComponent = playerCharacter->GetHealthComponent();
    return (healthComponent && healthComponent->HealHealth(fHealingAmount));
}