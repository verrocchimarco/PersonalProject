// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableBase.h"
#include "RestingPoint.generated.h"

/**
 * 
 */
class ABaseCharacter;
class APlayerStart;
UCLASS()
class INNERSANCTUM_API ARestingPoint : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	private:
		ABaseCharacter* playerCharacter;
	protected:
		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category ="Interaction Params", meta=(DisplayName="Prompt Description"))
		FText PromptDescription;
		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category ="Interaction Params", meta=(DisplayName="Interaction Enabled"))
		bool bCanBeInteracted = true;
		virtual void BeginPlay() override;
	public:
		UPROPERTY(EditAnywhere, Category="Respawn Point Details")
		APlayerStart* RespawnPoint;

		ARestingPoint();
		UFUNCTION(BlueprintCallable)
		void ActivateRespawnPoint();
};

