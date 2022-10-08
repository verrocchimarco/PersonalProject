// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UsableObjects.h"
#include "Weapon.generated.h"

/**
 * 
 */
UENUM(Blueprintable)
enum WeaponType
{
	Melee     UMETA(DisplayName = "Melee"),
	Ranged    UMETA(DisplayName = "Ranged"),
	Throwable UMETA(DisplayName = "Throwable"),
};
UCLASS(Blueprintable)
class INNERSANCTUM_API AWeapon : public AUsableObjects
{
	GENERATED_BODY()
	private:
	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Stats", meta=(DisplayName="Weapon Type"))
		TEnumAsByte<WeaponType> eWeaponType;
		virtual void BeginPlay() override;
	public:
		AWeapon();
		virtual bool UseItem();
};
