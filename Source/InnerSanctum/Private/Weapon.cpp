// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon()
{
    sInventoryDetails.sItemType = ItemCategory::Weapon;
}
void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

bool AWeapon::UseItem()
{
    Super::UseItem();
    UE_LOG(LogTemp, Display, TEXT("I'm a weapon though"));
    return false;
}