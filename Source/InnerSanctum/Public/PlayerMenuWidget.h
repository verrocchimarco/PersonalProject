// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class INNERSANCTUM_API UPlayerMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
		UFUNCTION(BlueprintImplementableEvent)
		void RequestInventoryUpdate();
};
