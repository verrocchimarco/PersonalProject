// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UENUM()
enum InteractionType
{
	SpaceButton 		UMETA(DisplayName = "SpaceButton"),
	EButton     		UMETA(DisplayName = "EButton"),
	DualInteraction		UMETA(DisplayName = "Dual Interaction")
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
		// Make item glow if in range
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetVisibilityGlow();
		// Return if the item can be interacted with
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool IsInteractionActive();
		// Return which button or buttons can be used to interact with the actor
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		InteractionType GetInteractionType();
		// Execute an interaction using a certain button
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DoInteract(AActor* InteractingActor, InteractionType ButtonPressed);
		// Get prompt description linked to button E
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetPromptDescriptionE();
		// Get prompt description linked to button Space
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetPromptDescriptionSpace();
};
