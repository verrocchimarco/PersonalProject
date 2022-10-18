// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UENUM()
enum InteractionType
{
	SpaceButton UMETA(DisplayName = "SpaceButton"),
	EButton     UMETA(DisplayName = "EButton"),
};

class AActor;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INNERSANCTUM_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetVisibilityGlow();
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DoInteract(AActor* InteractingActor);
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetPromptDescription();
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		InteractionType GetInteractionType();
};
