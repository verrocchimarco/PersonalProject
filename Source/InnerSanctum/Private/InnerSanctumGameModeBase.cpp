// Copyright Epic Games, Inc. All Rights Reserved.


#include "InnerSanctumGameModeBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProtagonistCharacter.h"
#include "HealthComponent.h"

void AInnerSanctumGameModeBase::BeginPlay()
{
    protagonistCharacter = Cast<AProtagonistCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
    protagonistCharacter->GetHealthComponent()->OnDeathDelegate.AddDynamic(this,&AInnerSanctumGameModeBase::ActorKilled);
}
void AInnerSanctumGameModeBase::ActorKilled()
{
    UE_LOG(LogTemp, Display, TEXT("He dead!"));
    RestartPlayer(protagonistCharacter->GetController());
}