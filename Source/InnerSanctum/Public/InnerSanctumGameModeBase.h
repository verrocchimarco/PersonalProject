// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InnerSanctumGameModeBase.generated.h"
/**
 * 
 */
class AProtagonistCharacter;
class ABaseCharacter;
class AInteractableBase;
UCLASS()
class INNERSANCTUM_API AInnerSanctumGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	private:
		AProtagonistCharacter* protagonistCharacter;
		UPROPERTY(EditAnywhere, Category="Player Backpack Pickup")
		TSubclassOf<AInteractableBase> LostBackpackObjectClass;
		AInteractableBase* LostBackpackObject;
	protected:
		virtual void BeginPlay() override;
		void SetLostBackpack();
	public:
		UFUNCTION()
		void ActorKilled(ABaseCharacter* deadCharacter);
		UFUNCTION(BlueprintCallable)
		void PlayerDeathNotification(AProtagonistCharacter* deadPlayer);
};
