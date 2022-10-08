// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UsableObjects.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

    private:
        UPROPERTY(EditDefaultsOnly, Category="Pockets")
        int PocketsInventorySize = 3;
        UPROPERTY(VisibleAnywhere, Category="Pockets")
        int availablePocketsSpace;
        UPROPERTY(EditAnywhere, Category="Backpack")
        bool bHasBackpack = true;
        UPROPERTY(EditAnywhere, Category="Backpack")
        int BackpackInventorySize = 4;
        UPROPERTY(VisibleAnywhere, Category="Backpack")
        int availableBackpackSpace;
        TArray<AUsableObjects*> BackpackHeldItems;
        TArray<AUsableObjects*> PocketsHeldItems;
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
        UFUNCTION(BlueprintCallable, BlueprintPure)
        TArray<AUsableObjects*> GetPocketsHeldItems() const{ return PocketsHeldItems; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        TArray<AUsableObjects*> GetBackpackHeldItems() const{ return BackpackHeldItems; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int GetPocketsInventorySize() const{ return PocketsInventorySize; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        int GetBackpackInventorySize() const{ return BackpackInventorySize; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        bool hasBackpack() const{ return bHasBackpack; }
        UFUNCTION(BlueprintCallable, BlueprintPure)
        FInventoryItem GetEquippedItemData() { if(EquippedItem) return EquippedItem->GetInventoryDetails(); else return FInventoryItem() ; }
        AUsableObjects* GetEquippedItem() { return EquippedItem; }
        AUsableObjects* GetItemRefAtIndex(int index, bool isBackpackInventory);

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
        bool EquipItem(int index, bool isBackpackInventory);
        UFUNCTION(BlueprintCallable)
        void UnequipItem();
        UFUNCTION(BlueprintCallable)
        bool SwitchInventoryItems(int firstItemIndex, bool isFirstItemInBackpack, int secondItemIndex, bool isSecondItemInBackpack);
};
