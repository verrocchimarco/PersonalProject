// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UsableObjects.h"
#include "Upgrade.h"
#include "Templates/Tuple.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

    private:
        UPROPERTY(EditDefaultsOnly, Category="Pockets")
        int PocketsInventorySize = 3;
        UPROPERTY(VisibleAnywhere, Category="Pockets")
        int fAvailablePocketsSpace;
        UPROPERTY(EditAnywhere, Category="Backpack")
        bool bHasBackpack = true;
        UPROPERTY(EditAnywhere, Category="Backpack")
        int BackpackInventorySize = 4;
        UPROPERTY(VisibleAnywhere, Category="Backpack")
        int fAvailableBackpackSpace;
        UStaticMeshComponent* CharacterBackpackMesh;
        UPROPERTY(EditAnywhere, Category="Backpack")
        UStaticMesh* BackpackMesh = nullptr;
        UPROPERTY()
        TArray<AUsableObjects*> TPocketsHeldItems;
        UPROPERTY()
        TArray<AUsableObjects*> TBackpackHeldItems;
        UPROPERTY()
        TArray<UUpgrade*> TUpgrades;
        const uint8_t nQuickItems = 4;
        TMap<int,TPair<int,bool>> TQuickItemsMap;
        AUsableObjects* EquippedItem = nullptr;
        TPair<int,bool> EquippedItemLocation = TPair<int,bool>(-1,false);
        bool bIsEquippedItemDrawn = false;

    public:	
        // Sets default values for this component's properties
        UInventoryComponent();

    protected:
        // Called when the game starts
        virtual void BeginPlay() override;
        void GameInstanceInit();

    public:	
        // Called every frame
        virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
        
        // Getters
        UFUNCTION(BlueprintCallable, BlueprintPure)
        TArray<AUsableObjects*> GetPocketsHeldItems() const{ return TPocketsHeldItems; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        TArray<AUsableObjects*> GetBackpackHeldItems() const{ return TBackpackHeldItems; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int GetPocketsInventorySize() const{ return PocketsInventorySize; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int GetBackpackInventorySize() const{ return BackpackInventorySize; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int GetAvailableBackpackSpace() const { return fAvailableBackpackSpace; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int GetAvailablePocketSpace() const { return fAvailablePocketsSpace; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        bool hasBackpack() const { return bHasBackpack; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        FInventoryItem  GetEquippedItemData() const { if(EquippedItem) return EquippedItem->GetInventoryDetails(); else return FInventoryItem() ; }
        TMap<int,TPair<int,bool>> GetQuickItemsMap() const { return TQuickItemsMap; }
        
        // Equipped item getters
        UFUNCTION(BlueprintCallable, BlueprintPure)
        AUsableObjects* GetEquippedItem() const { return EquippedItem; }
        UFUNCTION(BlueprintCallable)
        TSubclassOf<AUsableObjects> GetItemAtIndex(int index, bool isBackpackInventory);
        AUsableObjects* GetItemRefAtIndex(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable, BlueprintPure)
        bool IsEquippedItemDrawn() const { return bIsEquippedItemDrawn; }
        TPair<int,bool> GetEquippedItemLocation() const { return EquippedItemLocation; }
        
        // Upgrades getters
        UFUNCTION(BlueprintCallable, BlueprintPure)
        TArray<UUpgrade*> GetUpgrades() const{ return TUpgrades; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        FInventoryItem GetUpgradeDetailsAtIndex(int index) const{ return (TUpgrades.IsValidIndex(index)) ? TUpgrades[index]->GetUpgradeInventoryDetails() : FInventoryItem(); }

        // Setters
        UFUNCTION(BlueprintCallable)
        bool SetPocketsInventorySize(int newSize);
        UFUNCTION(BlueprintCallable)
        bool SetBackpackInventorySize(int newSize);
        
        // Items Management
        UFUNCTION(BlueprintCallable)
        bool AddItem(AUsableObjects* newItem);
        UFUNCTION(BlueprintCallable)
        bool RemoveItem(int index, bool isBackpackInventory, bool shouldSpawnPickup=true);
        UFUNCTION(BlueprintCallable)
        bool UseItemAtIndex(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        bool SwitchInventoryItems(int firstItemIndex, bool isFirstItemInBackpack, int secondItemIndex, bool isSecondItemInBackpack);
        
        // Equipped item Management
        UFUNCTION(BlueprintCallable)
        bool EquipItem(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        void UnequipItem();
        bool UseEquippedItem();
        UFUNCTION(BlueprintCallable, BlueprintPure)
        bool IsEquippedItem(int index, bool isBackpackInventory);
        void ToggleDrawEquippedItem();
        
        // Upgrades Management
        UFUNCTION(BlueprintCallable)
        bool AddUpgrade(UUpgrade* newUpgrade);
        UFUNCTION(BlueprintCallable)
        bool RemoveUpgrade(int index);

        // Backpack availability
        UFUNCTION(BlueprintCallable)
        void RemoveBackpack();
        UFUNCTION(BlueprintCallable)
        void EnableBackpack();
        UFUNCTION(BlueprintCallable)
        void SetBackpackMesh(UStaticMesh* newBackpackMesh);
        UFUNCTION(BlueprintCallable,BlueprintPure)
        UStaticMesh* GetBackpackMesh() const { return BackpackMesh;}

        // Item Handling
        UFUNCTION(BlueprintCallable,BlueprintPure)
        bool CanBeDropped(ItemCategory itemCategory) const { return itemCategory == HealingItem || itemCategory == Weapon;};
        UFUNCTION(BlueprintCallable,BlueprintPure)
        bool CanBeEquipped(ItemCategory itemCategory) const { return itemCategory == HealingItem || itemCategory == Weapon;};
        UFUNCTION(BlueprintCallable,BlueprintPure)
        bool CanBeUsed(ItemCategory itemCategory) const { return itemCategory == HealingItem || itemCategory == KeyItem || itemCategory == GenericItem;};
        
        // QuickItem Management
        UFUNCTION(BlueprintCallable)
        bool SetQuickItem(int quickItemIndex, int itemIndex, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        bool UnsetQuickItem(int quickItemIndex);
        UFUNCTION(BlueprintCallable)
        bool EquipQuickItem(int quickItemIndex);
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int  GetQuickItemIndex(int itemIndex, bool isBackpackInventory);
};
