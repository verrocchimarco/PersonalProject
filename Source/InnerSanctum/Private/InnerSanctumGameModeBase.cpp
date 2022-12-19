// Copyright Epic Games, Inc. All Rights Reserved.


#include "InnerSanctumGameModeBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProtagonistCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractableBase.h"
#include "InnerSanctumGameInstance.h"

void AInnerSanctumGameModeBase::BeginPlay()
{
    UE_LOG(LogTemp, Display, TEXT("GameMode: BeginPlay"));
    protagonistCharacter = Cast<AProtagonistCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
    if(protagonistCharacter)
    {
        protagonistCharacter->GetHealthComponent()->OnDeathDelegate.AddDynamic(this,&AInnerSanctumGameModeBase::ActorKilled);
    }
    SetLostBackpack();
}
void AInnerSanctumGameModeBase::SetLostBackpack()
{
    UInnerSanctumGameInstance* gameInstance = Cast<UInnerSanctumGameInstance>(GetWorld()->GetGameInstance());
    UStaticMesh* backpackMesh;
    UE_LOG(LogTemp, Display, TEXT("GameMode: Requesting backpack info from GameInstance"));
    if(!(gameInstance->GetPlayerHasBackpack()))
    {
        UE_LOG(LogTemp, Display, TEXT("GameMode: Player doesn't have a backpack"));
        backpackMesh=gameInstance->GetLostBackpackMesh();
        if(backpackMesh)
        {
            UE_LOG(LogTemp, Display, TEXT("GameMode: Retrieved backpack mesh: [%s]"),*(backpackMesh->GetName()));
            FTransform SpawnTransform;
            SpawnTransform.SetLocation(gameInstance->GetDeathLocation());
            LostBackpackObject = Cast<AInteractableBase>(GetWorld()->SpawnActorDeferred<AInteractableBase>(LostBackpackObjectClass,SpawnTransform));
            LostBackpackObject->SetEmissiveMesh(backpackMesh);
            LostBackpackObject->FinishSpawning(SpawnTransform);
        }
    }

}
void AInnerSanctumGameModeBase::ActorKilled(ABaseCharacter* deadCharacter)
{
    UE_LOG(LogProcess, Display, TEXT("GameMode: detected character's death"));
    if(Cast<AProtagonistCharacter>(deadCharacter))
    {
        // If the player has died, register event to gameInstance and then wait for the death animation to end
        UInnerSanctumGameInstance* gameInstance = Cast<UInnerSanctumGameInstance>(GetWorld()->GetGameInstance());
        gameInstance->ActorKilled(deadCharacter);
        Cast<UCharacterMovementComponent>(deadCharacter->GetMovementComponent())->DisableMovement();
        deadCharacter->DisableInput(Cast<APlayerController>(deadCharacter->GetController()));
    }
}

void AInnerSanctumGameModeBase::PlayerDeathNotification(AProtagonistCharacter* deadPlayer)
{
    APlayerController* controller = Cast<APlayerController>(deadPlayer->GetController());
    deadPlayer->DetachFromControllerPendingDestroy();
	deadPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    deadPlayer->Destroy();
    UE_LOG(LogTemp, Display, TEXT("GameMode: Restarting Level!"));
    controller->RestartLevel();
    // RestartPlayer(controller);
    ResetLevel();
}