// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UsableObjects.h"
#include "Upgrade.h"
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
        TArray<AUsableObjects*> TBackpackHeldItems;
        TArray<AUsableObjects*> TPocketsHeldItems;
        TArray<UUpgrade*> TUpgrades;
        AUsableObjects* EquippedItem = nullptr;

    public:	
        // Sets default values for this component's properties
        UInventoryComponent();

    protected:
        // Called when the game starts
        virtual void BeginPlay() override;

    public:	
        // Called every frame
        virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
        
        // Getters
            // Sub inventory getters
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
        bool hasBackpack() const{ return bHasBackpack; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
            // Equipped item getters
        FInventoryItem GetEquippedItemData() { if(EquippedItem) return EquippedItem->GetInventoryDetails(); else return FInventoryItem() ; }
        AUsableObjects* GetEquippedItem() { return EquippedItem; }
        AUsableObjects* GetItemRefAtIndex(int index, bool isBackpackInventory);
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
        bool UseItemAtIndex(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        bool RemoveItemAtIndex(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        TSubclassOf<AUsableObjects> GetItemAtIndex(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        bool SwitchInventoryItems(int firstItemIndex, bool isFirstItemInBackpack, int secondItemIndex, bool isSecondItemInBackpack);
        // Equipped item Management
        UFUNCTION(BlueprintCallable)
        bool EquipItem(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        void UnequipItem();
        // Upgrades Management
        UFUNCTION(BlueprintCallable)
        bool AddUpgrade(UUpgrade* newUpgrade);
        UFUNCTION(BlueprintCallable)
        bool RemoveUpgrade(int index);
};
