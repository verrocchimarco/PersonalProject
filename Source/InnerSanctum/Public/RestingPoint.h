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
class INNERSANCTUM_API ARestingPoint : public AInteractableBase
{
	GENERATED_BODY()
	private:
		ABaseCharacter* playerCharacter;
		UPROPERTY(VisibleAnywhere, Category="Respawn Point Details")
		bool bIsHealing;
	protected:
		virtual void BeginPlay() override;
	public:
		UPROPERTY(EditAnywhere, Category="Respawn Point Details")
		APlayerStart* RespawnPoint;

		ARestingPoint();
		UFUNCTION(BlueprintCallable)
		void ActivateRespawnPoint();
};
