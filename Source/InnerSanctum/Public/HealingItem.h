// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UsableObjects.h"
#include "HealingItem.generated.h"

/**
 * 
 */
UCLASS()
class INNERSANCTUM_API AHealingItem : public AUsableObjects
{
	GENERATED_BODY()
	private:
	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Healing", meta=(DisplayName="Healing Amount"))
		float fHealingAmount;
		virtual void BeginPlay() override;
	public:
		AHealingItem();
		virtual bool UseItem();
};
