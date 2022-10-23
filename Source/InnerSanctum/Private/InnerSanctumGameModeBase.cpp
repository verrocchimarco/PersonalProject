// Copyright Epic Games, Inc. All Rights Reserved.


#include "InnerSanctumGameModeBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProtagonistCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "InnerSanctumGameInstance.h"

void AInnerSanctumGameModeBase::BeginPlay()
{
    protagonistCharacter = Cast<AProtagonistCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
    protagonistCharacter->GetHealthComponent()->OnDeathDelegate.AddDynamic(this,&AInnerSanctumGameModeBase::ActorKilled);
    UE_LOG(LogTemp, Display, TEXT("GameMode: BeginPlay"));
}
void AInnerSanctumGameModeBase::ActorKilled(ABaseCharacter* deadCharacter)
{
    UE_LOG(LogProcess, Display, TEXT("GameMode: detected character's death"));
    UInnerSanctumGameInstance* gameInstance = Cast<UInnerSanctumGameInstance>(GetWorld()->GetGameInstance());
    gameInstance->ActorKilled(deadCharacter);
    APlayerController* controller = Cast<APlayerController>(deadCharacter->GetController());
    deadCharacter->DetachFromControllerPendingDestroy();
	deadCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    UE_LOG(LogTemp, Display, TEXT("He dead!"));
    controller->RestartLevel();
    ResetLevel();
}