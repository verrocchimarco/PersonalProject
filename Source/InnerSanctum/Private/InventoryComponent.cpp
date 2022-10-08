// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Kismet/KismetArrayLibrary.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	PocketsHeldItems.Reserve(PocketsInventorySize);
	BackpackHeldItems.Reserve(BackpackInventorySize);
	bHasBackpack = true;
	availablePocketsSpace = PocketsInventorySize;
	availableBackpackSpace = BackpackInventorySize;
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponent::AddItem(AUsableObjects* newItem)
{
	int itemSlotSize = newItem->GetInventoryDetails().iSlotSize;
	if( availablePocketsSpace - itemSlotSize >= 0 )
	{
		PocketsHeldItems.Emplace(newItem);
		availablePocketsSpace -= itemSlotSize;
	}
	else if ( availableBackpackSpace - itemSlotSize >= 0 )
	{
		BackpackHeldItems.Emplace(newItem);
		availableBackpackSpace -= itemSlotSize;
	}
	else
		return false;
	return true;
}

AUsableObjects* UInventoryComponent::GetItemRefAtIndex(int index, bool isBackpackInventory)
{
	if(!isBackpackInventory && PocketsHeldItems.IsValidIndex(index))
	{
		return PocketsHeldItems[index];
	}
	else if(isBackpackInventory && bHasBackpack && BackpackHeldItems.IsValidIndex(index))
	{
		return BackpackHeldItems[index];
	}
	else
	{
		return nullptr;
	}
}

bool UInventoryComponent::UseItemAtIndex(int index, bool isBackpackInventory)
{
	bool usageSuccessful = false;
	if(!isBackpackInventory && PocketsHeldItems.IsValidIndex(index))
	{
		if(PocketsHeldItems[index]->UseItem())
		{
			availablePocketsSpace += PocketsHeldItems[index]->GetInventoryDetails().iSlotSize;
			PocketsHeldItems.RemoveAtSwap(index);
			usageSuccessful = true;
			UE_LOG(LogTemp, Display, TEXT("Removing Item at %d, available pocket space %d"),index,availablePocketsSpace);
		}
	}
	else if(isBackpackInventory && bHasBackpack && BackpackHeldItems.IsValidIndex(index))
	{
		if(BackpackHeldItems[index]->UseItem())
		{
			availableBackpackSpace += BackpackHeldItems[index]->GetInventoryDetails().iSlotSize;
			BackpackHeldItems.RemoveAtSwap(index);
			usageSuccessful = true;
		}
	}
	return usageSuccessful;
}


bool UInventoryComponent::RemoveItemAtIndex(int index, bool isBackpackInventory)
{
	bool removalSuccessful = false;
	AUsableObjects* ObjectReference = nullptr;
	if(!isBackpackInventory && PocketsHeldItems.IsValidIndex(index))
	{
		availablePocketsSpace += PocketsHeldItems[index]->GetInventoryDetails().iSlotSize;
		removalSuccessful = true;
		ObjectReference = PocketsHeldItems[index];
		PocketsHeldItems.RemoveAtSwap(index);
	}
	else if(isBackpackInventory && bHasBackpack && BackpackHeldItems.IsValidIndex(index))
	{
		
		availableBackpackSpace += BackpackHeldItems[index]->GetInventoryDetails().iSlotSize;
		removalSuccessful = true;
		ObjectReference = BackpackHeldItems[index];
		BackpackHeldItems.RemoveAtSwap(index);
	}
	if (EquippedItem == ObjectReference)
	{
		UnequipItem();
	}
	if(ObjectReference)
	{
		ObjectReference->Destroy();
	}
	return removalSuccessful;
}

TSubclassOf<AUsableObjects> UInventoryComponent::GetItemAtIndex(int index, bool isBackpackInventory)
{
	TSubclassOf<AUsableObjects> ObjectDefaults = nullptr;
	if(!isBackpackInventory && PocketsHeldItems.IsValidIndex(index))
	{
		ObjectDefaults = PocketsHeldItems[index]->GetClass();
	}
	else if(isBackpackInventory && bHasBackpack && BackpackHeldItems.IsValidIndex(index))
	{
		ObjectDefaults = BackpackHeldItems[index]->GetClass();
	}
	return ObjectDefaults;
}
bool UInventoryComponent::EquipItem(int index, bool isBackpackInventory)
{
	bool equipSuccessful = false;
	if(!isBackpackInventory && PocketsHeldItems.IsValidIndex(index))
	{
		EquippedItem = PocketsHeldItems[index];
		equipSuccessful = true;
	}
	else if(isBackpackInventory && bHasBackpack && BackpackHeldItems.IsValidIndex(index))
	{
		EquippedItem = BackpackHeldItems[index];
		equipSuccessful = true;
	}
	return equipSuccessful = true;;
}

void UInventoryComponent::UnequipItem()
{
	EquippedItem = nullptr;
}

bool UInventoryComponent::SwitchInventoryItems(int firstItemIndex, bool isFirstItemInBackpack, int secondItemIndex, bool isSecondItemInBackpack)
{
	bool operationSuccessful = false;
	AUsableObjects *pocketsItem, *backpackItem;
	int swapSizeResultPockets, swapSizeResultBackpack;
	auto getItem = [](int& index, TArray<AUsableObjects*>& inventory){ return (inventory.IsValidIndex(index) ? inventory[index] : nullptr);	};
	int logb1 = isFirstItemInBackpack ? 1 : 0;
	int logb2 = isSecondItemInBackpack ? 1 : 0;
	UE_LOG(LogTemp, Display, TEXT("Swap requested.\nItem 1 index:[%d] isBackpack[%d]\nItem 2 index:[%d] isBackpack[%d]"),firstItemIndex,logb1,secondItemIndex,logb2);
	// Return true if the items are in the same sub inventory. Switch useless, but perform it because otherwise it looks bad on the UI. Goddamn gamers
	if( isFirstItemInBackpack && isSecondItemInBackpack)
	{
		UE_LOG(LogTemp, Display, TEXT("Swap not needed. Both in backpack"));
		BackpackHeldItems.Swap(firstItemIndex,secondItemIndex);
		return true;
	}
	if (!isFirstItemInBackpack && !isSecondItemInBackpack)
	{
		UE_LOG(LogTemp, Display, TEXT("Swap not needed. Both in pockets"));
		PocketsHeldItems.Swap(firstItemIndex,secondItemIndex);
		return true;
	}
	// Stop swap if both indexes are -1
	if (firstItemIndex < 0 && secondItemIndex < 0 )
	{
		UE_LOG(LogTemp, Display, TEXT("Swap impossible. Both don't exist"));
		return false;
	}
	// Don't proceed if one of the items is in the backpack when the backpack isn't available
	if( (isFirstItemInBackpack || isSecondItemInBackpack) && !bHasBackpack )
	{
		UE_LOG(LogTemp, Display, TEXT("Swap impossible. Backpack not available"));
		return false;
	}
	// Get items from the 2 different subinventories
	pocketsItem = (isFirstItemInBackpack) ? getItem(secondItemIndex,PocketsHeldItems) : getItem(firstItemIndex,PocketsHeldItems);
	backpackItem = (isFirstItemInBackpack) ? getItem(firstItemIndex,BackpackHeldItems) : getItem(secondItemIndex,BackpackHeldItems);
	// If both items exist, evaluate change of available space and swap them
	if(pocketsItem && backpackItem)
	{
		swapSizeResultPockets = availablePocketsSpace - pocketsItem->GetInventoryDetails().iSlotSize + backpackItem->GetInventoryDetails().iSlotSize;
		swapSizeResultBackpack = availableBackpackSpace - backpackItem->GetInventoryDetails().iSlotSize + pocketsItem->GetInventoryDetails().iSlotSize;
		// Execute swap if size allows it
		if( (swapSizeResultBackpack >= 0) && (swapSizeResultPockets >= 0) )
		{
			UE_LOG(LogTemp, Display, TEXT("Executing swap, both items exist"));
			BackpackHeldItems.Remove(backpackItem);
			PocketsHeldItems.Remove(pocketsItem);
			BackpackHeldItems.Add_GetRef(pocketsItem);
			PocketsHeldItems.Add_GetRef(backpackItem);
			availablePocketsSpace = swapSizeResultPockets;
			availableBackpackSpace = swapSizeResultBackpack;
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Swap impossible. Available size not sufficient. Pockets projected size [%d] Backpack projected size [%d]"), swapSizeResultPockets,swapSizeResultBackpack);
			return false;
		}
	}
	// If only the pockets item exist, evaluate the available space in the backpack inventory and execute the add
	else if (pocketsItem)
	{
		swapSizeResultBackpack = availableBackpackSpace - pocketsItem->GetInventoryDetails().iSlotSize;
		if( swapSizeResultBackpack >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Executing swap, only pockets item exists"));
			BackpackHeldItems.Add_GetRef(pocketsItem);
			PocketsHeldItems.Remove(pocketsItem);
			availableBackpackSpace = swapSizeResultBackpack;
			availablePocketsSpace += pocketsItem->GetInventoryDetails().iSlotSize;
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Swap impossible. Available size not sufficient. Backpack projected size [%d]"),swapSizeResultBackpack);
			return false;
		}
	}
	// If the backpack item exist, evaluate the available space in the pockets inventory and execute the add
	else if (backpackItem)
	{
		swapSizeResultPockets = availablePocketsSpace - backpackItem->GetInventoryDetails().iSlotSize;
		if( swapSizeResultPockets >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Executing swap, only backpack item exists"));
			PocketsHeldItems.Add_GetRef(pocketsItem);
			BackpackHeldItems.Remove(pocketsItem);
			availablePocketsSpace = swapSizeResultPockets;
			availableBackpackSpace += backpackItem->GetInventoryDetails().iSlotSize;
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Swap impossible. Available size not sufficient. Pockets projected size [%d]"),swapSizeResultPockets);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Swap impossible. Both items don't exist"));
		return false;
	}
}