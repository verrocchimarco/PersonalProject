// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NotesDataAsset.h"
#include "Containers/Map.h"
#include "GameFramework/PlayerStart.h"
#include "InnerSanctumGameInstance.generated.h"
/**
 * 
 */
class AUsableObjects;
class UUpgrade;
class AProtagonistCharacter;
class ABaseCharacter;
class AInnerSanctumGameModeBase;
UCLASS()
class INNERSANCTUM_API UInnerSanctumGameInstance : public UGameInstance
{
	GENERATED_BODY()
	protected:
		// Notes persistence
		UPROPERTY(EditDefaultsOnly)
		UNotesDataAsset* NotesCollection;
		TMap<int32,TPair<bool,bool>> MNotesStatus; // index <found,read>
		TArray<FText> LocationsList;
		// Player inventory
		bool bPlayerHasBackpack = true;
		FVector DeathLocation;
		UStaticMesh* LostBackpackMesh = nullptr;
		UPROPERTY()
		TArray<TSubclassOf<AUsableObjects>> PlayerPocketsItems;
		UPROPERTY()
        TArray<TSubclassOf<AUsableObjects>> PlayerBackpackItems;
		UPROPERTY()
        TArray<TSubclassOf<UUpgrade>> PlayerUpgrades;
		TPair<int,bool> EquippedItemLocation = TPair<int,bool>(-1,false);
		TMap<int,TPair<int,bool>> PlayerQuickItems;
		UPROPERTY()
		FName ActiveRespawnPointTag = TEXT("EntryPoint");
	public:
		virtual void Init() override;
		// Notes management
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<FNoteData>& GetNotesCollection() const { return NotesCollection->notes; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<FText> GetLocationsList() const { return LocationsList; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<int32> GetFoundNotes() const;
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<int32> GetReadFoundNotes() const;
		UFUNCTION(BlueprintCallable)
		bool SetNoteAsFound(int32 index);
		UFUNCTION(BlueprintCallable)
		bool SetNoteAsRead(int32 index);
		// Inventory management
		TArray<TSubclassOf<AUsableObjects>> GetPlayerPocketsItems() 	const { return PlayerPocketsItems; }
        TArray<TSubclassOf<AUsableObjects>> GetPlayerBackpackItems() 	const { return PlayerBackpackItems;}
        TArray<TSubclassOf<UUpgrade>> 		GetPlayerUpgrades() 		const { return PlayerUpgrades;}
		TPair<int,bool> 					GetEquippedItemLocation() 	const { return EquippedItemLocation;}
		TMap<int,TPair<int,bool>>			GetPlayerQuickItems()		const { return PlayerQuickItems; }
		bool								GetPlayerHasBackpack()		const { return bPlayerHasBackpack;}
		void								SavePlayerInventory(AProtagonistCharacter* protagonistCharacter, bool isDead);
		UFUNCTION()
		void ActorKilled(ABaseCharacter* deadCharacter);
		UFUNCTION(BlueprintCallable,BlueprintPure)
		UStaticMesh* GetLostBackpackMesh() const { return LostBackpackMesh; }
		UFUNCTION(BlueprintCallable,BlueprintPure)
		FVector GetDeathLocation() const { return DeathLocation; }
		void SetRespawnPointTag(FName newRestartPoint) { ActiveRespawnPointTag = newRestartPoint; }
		void ResetRespawnPointTag() { ActiveRespawnPointTag = TEXT("EntryPoint"); }
		UFUNCTION(BlueprintCallable,BlueprintPure)
		FName GetRespawnPointTag() const { return ActiveRespawnPointTag;}
};
