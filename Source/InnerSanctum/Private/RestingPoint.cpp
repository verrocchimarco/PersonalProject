// Fill out your copyright notice in the Description page of Project Settings.


#include "RestingPoint.h"
#include "BaseCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "InnerSanctumGameInstance.h"
ARestingPoint::ARestingPoint()
{
    RespawnPoint = nullptr;
}

void ARestingPoint::BeginPlay()
{
    Super::BeginPlay();
    playerCharacter = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
}

void ARestingPoint::ActivateRespawnPoint()
{
    // playerCharacter->BeginResting();
    Cast<UInnerSanctumGameInstance>(GetWorld()->GetGameInstance())->SetRespawnPointTag(RespawnPoint->PlayerStartTag);
}
