// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InnerSanctumGameModeBase.generated.h"

/**
 * 
 */
class AProtagonistCharacter;
UCLASS()
class INNERSANCTUM_API AInnerSanctumGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	private:
		AProtagonistCharacter* protagonistCharacter;
	protected:
		virtual void BeginPlay() override;
	public:
		UFUNCTION()
		void ActorKilled();
};
