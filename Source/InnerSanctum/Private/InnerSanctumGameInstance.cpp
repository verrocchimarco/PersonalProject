// Fill out your copyright notice in the Description page of Project Settings.


#include "InnerSanctumGameInstance.h"
#include "NotesDataAsset.h"
#include "ProtagonistCharacter.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"

void UInnerSanctumGameInstance::Init()
{
    Super::Init();
    int32 index = 0;
    for(auto& noteAsset : NotesCollection->notes)
    {
        MNotesStatus.Add(index++, TPair<bool,bool>(false,false));
        if(!LocationsList.ContainsByPredicate([&](const FText location){ return !location.CompareToCaseIgnored(noteAsset.Location); }))
        {
            LocationsList.Emplace(noteAsset.Location);
        }
    }
}

TArray<int32> UInnerSanctumGameInstance::GetFoundNotes() const
{
    TArray<int32> result;
    TArray<TPair<bool,bool>> MapNotesStatusValues;
    MNotesStatus.GenerateValueArray(MapNotesStatusValues);
    int index = 0;
    for(const TPair<bool,bool>& status : MapNotesStatusValues)
    {
        if(status.Key)
        {
            result.Emplace(index);
        }
        index++;
    }
    return result;
}

TArray<int32> UInnerSanctumGameInstance::GetReadFoundNotes() const
{
    TArray<int32> result;
    TArray<TPair<bool,bool>> MapNotesStatusValues;
    MNotesStatus.GenerateValueArray(MapNotesStatusValues);
    int index = 0;
    for(const TPair<bool,bool>& status : MapNotesStatusValues)
    {
        if(status.Key && status.Value)
        {
            result.Emplace(index);
        }
        index++;
    }
    return result;
}

bool UInnerSanctumGameInstance::SetNoteAsFound(int32 index)
{
    if(MNotesStatus.Contains(index))
    {
        MNotesStatus[index].Key = true;
        return true;
    }
    else
    {
        return false;
    }
}

bool UInnerSanctumGameInstance::SetNoteAsRead(int32 index)
{
    if(MNotesStatus.Contains(index))
    {
        MNotesStatus[index].Value = true;
        return true;
    }
    else
    {
        return false;
    }
}

void UInnerSanctumGameInstance::ActorKilled(ABaseCharacter* deadCharacter)
{
    AProtagonistCharacter* protagonistCharacter = Cast<AProtagonistCharacter>(deadCharacter);
    if(protagonistCharacter)
    {
        SavePlayerInventory(protagonistCharacter,true);
    }
}

void UInnerSanctumGameInstance::SavePlayerInventory(AProtagonistCharacter* protagonistCharacter, bool isDead)
{
    UInventoryComponent* playerInventory = protagonistCharacter->GetInventoryComponent();
    PlayerPocketsItems.Empty();
    PlayerBackpackItems.Empty();
    PlayerUpgrades.Empty();
    PlayerQuickItems.Empty();
    for(auto& pocketItem : playerInventory->GetPocketsHeldItems())
    {
        UE_LOG(LogProcess, Display, TEXT("GameInstance: copying pocket item %s"),*(pocketItem->GetName()));
        PlayerPocketsItems.Add(pocketItem->GetClass());
    }
    for(auto& backpackItem : playerInventory->GetBackpackHeldItems())
    {
        UE_LOG(LogProcess, Display, TEXT("GameInstance: copying backpack item %s"),*(backpackItem->GetName()));
        PlayerBackpackItems.Add(backpackItem->GetClass());
    }
    for(auto& upgrade : playerInventory->GetUpgrades())
    {
        PlayerUpgrades.Add(upgrade->GetClass());
        UE_LOG(LogProcess, Display, TEXT("GameInstance: copying upgrade item %s"),*(upgrade->GetName()));
    }
    EquippedItemLocation = playerInventory->GetEquippedItemLocation();
    // Store backpack mesh and death location, if player died. GameMode will use this data
    if(isDead)
    {
        UE_LOG(LogTemp, Display, TEXT("GameInstance: Character died"));
        // If the player has lost the backpack in this instance, load the location. Otherwise don't change it (player has died and lost the backpack already)
        if(playerInventory->hasBackpack())
        {
            DeathLocation = protagonistCharacter->GetActorLocation();
        }
        LostBackpackMesh = playerInventory->GetBackpackMesh();
        bPlayerHasBackpack = false;
        UE_LOG(LogTemp, Display, TEXT("GameInstance: Obtained character's current backpack: [%s]"), *(LostBackpackMesh->GetName()));
    }
    else
    {
        bPlayerHasBackpack = playerInventory->hasBackpack();
        LostBackpackMesh = nullptr;
    }
    // Store player quick items selection. Remove backpack items from the quick items
    PlayerQuickItems = playerInventory->GetQuickItemsMap();
    for(auto& quickItem : PlayerQuickItems)
    {
        if(quickItem.Value.Value == true )
        {
            quickItem.Value.Key = -1; // invalid index
            quickItem.Value.Value = false; // default (item is not in backpack)
        }
    }
}