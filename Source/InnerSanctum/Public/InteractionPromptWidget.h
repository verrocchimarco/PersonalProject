// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class INNERSANCTUM_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
		UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void SetPromptDescriptionE(const FText & Description);
		UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void SetPromptDescriptionSpace(const FText & Description);
};
