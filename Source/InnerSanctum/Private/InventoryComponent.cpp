// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Kismet/KismetArrayLibrary.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	TPocketsHeldItems.Reserve(PocketsInventorySize);
	TBackpackHeldItems.Reserve(BackpackInventorySize);
	bHasBackpack = true;
	fAvailablePocketsSpace = PocketsInventorySize;
	fAvailableBackpackSpace = BackpackInventorySize;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

 bool UInventoryComponent::SetPocketsInventorySize(int newSize)
 {
	if(newSize >= PocketsInventorySize)
	{
		fAvailablePocketsSpace += newSize - PocketsInventorySize;
		PocketsInventorySize = newSize;
		return true;
	}
	else
	{
		return false;
	}
 }
bool UInventoryComponent::SetBackpackInventorySize(int newSize)
 {
	if(newSize >= BackpackInventorySize)
	{
		fAvailableBackpackSpace += newSize - BackpackInventorySize;
		BackpackInventorySize = newSize;
		return true;
	}
	else
	{
		return false;
	}
 }

bool UInventoryComponent::AddItem(AUsableObjects* newItem)
{
	int itemSlotSize = newItem->GetInventoryDetails().iSlotSize;
	if( fAvailablePocketsSpace - itemSlotSize >= 0 )
	{
		TPocketsHeldItems.Emplace(newItem);
		fAvailablePocketsSpace -= itemSlotSize;
	}
	else if ( fAvailableBackpackSpace - itemSlotSize >= 0 )
	{
		TBackpackHeldItems.Emplace(newItem);
		fAvailableBackpackSpace -= itemSlotSize;
	}
	else
		return false;
	return true;
}

AUsableObjects* UInventoryComponent::GetItemRefAtIndex(int index, bool isBackpackInventory)
{
	if(!isBackpackInventory && TPocketsHeldItems.IsValidIndex(index))
	{
		return TPocketsHeldItems[index];
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		return TBackpackHeldItems[index];
	}
	else
	{
		return nullptr;
	}
}

bool UInventoryComponent::UseItemAtIndex(int index, bool isBackpackInventory)
{
	bool usageSuccessful = false;
	AUsableObjects* usedObject = nullptr;
	if(!isBackpackInventory && TPocketsHeldItems.IsValidIndex(index))
	{
		// If usage is successful, eliminate object
		if(TPocketsHeldItems[index]->UseItem())
		{
			usedObject = TPocketsHeldItems[index];
			fAvailablePocketsSpace += TPocketsHeldItems[index]->GetInventoryDetails().iSlotSize;
			TPocketsHeldItems.RemoveAtSwap(index);
			usageSuccessful = true;
			UE_LOG(LogTemp, Display, TEXT("Removing Item at %d, available pocket space %d"),index,fAvailablePocketsSpace);
		}
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		if(TBackpackHeldItems[index]->UseItem())
		{
			usedObject = TBackpackHeldItems[index];
			fAvailableBackpackSpace += TBackpackHeldItems[index]->GetInventoryDetails().iSlotSize;
			TBackpackHeldItems.RemoveAtSwap(index);
			usageSuccessful = true;
			UE_LOG(LogTemp, Display, TEXT("Removing Item at %d, available backpack space %d"),index,fAvailableBackpackSpace);
		}
	}
	if(usageSuccessful)
	{
		// Unequip
		if(usedObject && EquippedItem == usedObject)
		{
			UnequipItem();
		}
		usedObject->Destroy();
	}
	return usageSuccessful;
}


bool UInventoryComponent::RemoveItemAtIndex(int index, bool isBackpackInventory, bool shouldSpawnPickup)
{
	bool removalSuccessful = false;
	AUsableObjects* ObjectReference = nullptr;
	UE_LOG(LogTemp, Display, TEXT("Remove index: %d"),index);
	if(!isBackpackInventory && TPocketsHeldItems.IsValidIndex(index))
	{
		UE_LOG(LogTemp, Display, TEXT("Item in pockets removal"));
		fAvailablePocketsSpace += TPocketsHeldItems[index]->GetInventoryDetails().iSlotSize;
		removalSuccessful = true;
		ObjectReference = TPocketsHeldItems[index];
		TPocketsHeldItems.RemoveAtSwap(index);
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		UE_LOG(LogTemp, Display, TEXT("Item in backpack removal"));
		fAvailableBackpackSpace += TBackpackHeldItems[index]->GetInventoryDetails().iSlotSize;
		removalSuccessful = true;
		ObjectReference = TBackpackHeldItems[index];
		TBackpackHeldItems.RemoveAtSwap(index);
	}
	else
	{
		return removalSuccessful;
	}
	if(ObjectReference)
	{
		// If the removed item is also the equipped one, unequip it
		if (EquippedItem == ObjectReference)
		{
			UE_LOG(LogTemp, Display, TEXT("Unequipping item"));
			UnequipItem();
		}
		if(shouldSpawnPickup)
		{
			// Spawn PickupActor
			FVector spawningLocation = GetOwner()->GetActorLocation();
			FActorSpawnParameters spawnParameters;
			spawningLocation.X += 25;
			GetWorld()->SpawnActor(ObjectReference->GetPickupClassReference(),&spawningLocation,0);
		}
		ObjectReference->Destroy();
	}
	return removalSuccessful;
}

TSubclassOf<AUsableObjects> UInventoryComponent::GetItemAtIndex(int index, bool isBackpackInventory)
{
	TSubclassOf<AUsableObjects> ObjectDefaults = nullptr;
	if(!isBackpackInventory && TPocketsHeldItems.IsValidIndex(index))
	{
		ObjectDefaults = TPocketsHeldItems[index]->GetClass();
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		ObjectDefaults = TBackpackHeldItems[index]->GetClass();
	}
	return ObjectDefaults;
}


bool UInventoryComponent::SwitchInventoryItems(int firstItemIndex, bool isFirstItemInBackpack, int secondItemIndex, bool isSecondItemInBackpack)
{
	AUsableObjects *pocketsItem, *backpackItem;
	int swapSizeResultPockets, swapSizeResultBackpack;
	auto getItem = [](int& index, TArray<AUsableObjects*>& inventory){ return (inventory.IsValidIndex(index) ? inventory[index] : nullptr);	};
	int logb1 = isFirstItemInBackpack ? 1 : 0;
	int logb2 = isSecondItemInBackpack ? 1 : 0;
	UE_LOG(LogTemp, Display, TEXT("Swap requested.\nItem 1 index:[%d] isBackpack[%d]\nItem 2 index:[%d] isBackpack[%d]"),firstItemIndex,logb1,secondItemIndex,logb2);
	// Return true if the items are in the same sub inventory. Switch useless, but perform it because otherwise it looks bad on the UI. Goddamn gamers
	if( (isFirstItemInBackpack && isSecondItemInBackpack) )
	{
		if(firstItemIndex >= 0 && secondItemIndex >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Swap not needed. Both in backpack"));
			TBackpackHeldItems.Swap(firstItemIndex,secondItemIndex);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Swap impossible. Both in backpack but one doesn't exist"));
			return false;
		}
	}
	if ( (!isFirstItemInBackpack && !isSecondItemInBackpack))
	{
		if(firstItemIndex >= 0 && secondItemIndex >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Swap not needed. Both in pockets"));
			TPocketsHeldItems.Swap(firstItemIndex,secondItemIndex);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Swap impossible. Both in items but one doesn't exist"));
			return false;
		}
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
	pocketsItem = (isFirstItemInBackpack) ? getItem(secondItemIndex,TPocketsHeldItems) : getItem(firstItemIndex,TPocketsHeldItems);
	backpackItem = (isFirstItemInBackpack) ? getItem(firstItemIndex,TBackpackHeldItems) : getItem(secondItemIndex,TBackpackHeldItems);
	// If both items exist, evaluate change of available space and swap them
	if(pocketsItem && backpackItem)
	{
		swapSizeResultPockets = fAvailablePocketsSpace + pocketsItem->GetInventoryDetails().iSlotSize - backpackItem->GetInventoryDetails().iSlotSize;
		swapSizeResultBackpack = fAvailableBackpackSpace + backpackItem->GetInventoryDetails().iSlotSize - pocketsItem->GetInventoryDetails().iSlotSize;
		// Execute swap if size allows it
		if( (swapSizeResultBackpack >= 0) && (swapSizeResultPockets >= 0) )
		{
			UE_LOG(LogTemp, Display, TEXT("Executing swap, both items exist"));
			TBackpackHeldItems.Remove(backpackItem);
			TPocketsHeldItems.Remove(pocketsItem);
			TBackpackHeldItems.Add_GetRef(pocketsItem);
			TPocketsHeldItems.Add_GetRef(backpackItem);
			fAvailablePocketsSpace = swapSizeResultPockets;
			fAvailableBackpackSpace = swapSizeResultBackpack;
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
		swapSizeResultBackpack = fAvailableBackpackSpace - pocketsItem->GetInventoryDetails().iSlotSize;
		if( swapSizeResultBackpack >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Executing swap, only pockets item exists"));
			TBackpackHeldItems.Add_GetRef(pocketsItem);
			TPocketsHeldItems.Remove(pocketsItem);
			fAvailableBackpackSpace = swapSizeResultBackpack;
			fAvailablePocketsSpace += pocketsItem->GetInventoryDetails().iSlotSize;
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
		swapSizeResultPockets = fAvailablePocketsSpace - backpackItem->GetInventoryDetails().iSlotSize;
		if( swapSizeResultPockets >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Executing swap, only backpack item exists"));
			TPocketsHeldItems.Add_GetRef(backpackItem);
			TBackpackHeldItems.Remove(backpackItem);
			fAvailablePocketsSpace = swapSizeResultPockets;
			fAvailableBackpackSpace += backpackItem->GetInventoryDetails().iSlotSize;
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


bool UInventoryComponent::EquipItem(int index, bool isBackpackInventory)
{
	bool equipSuccessful = false;
	AUsableObjects* newEquippedItem = nullptr;
	if(!isBackpackInventory && TPocketsHeldItems.IsValidIndex(index))
	{
		newEquippedItem = TPocketsHeldItems[index];
		equipSuccessful = true;
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		newEquippedItem = TBackpackHeldItems[index];
		equipSuccessful = true;
	}
	// If previously equipped item was being used, play switch animation
	if(equipSuccessful && newEquippedItem && EquippedItem && EquippedItem->IsActive())
	{
		EquippedItem->DisableItem();
		EquippedItem = newEquippedItem;
		EquippedItem->ActivateItem();
	}
	else
	{
		EquippedItem = newEquippedItem;
	}
	return equipSuccessful;
}

void UInventoryComponent::UnequipItem()
{
	if(EquippedItem && EquippedItem->IsActive())
	{
		EquippedItem->DisableItem();
	}
	EquippedItem = nullptr;
}

bool UInventoryComponent::UseEquippedItem()
{
	bool hasItemExpired = false;
	if(EquippedItem)
	{
		hasItemExpired = EquippedItem->UseItem();
		// If the item has been used and "destroyed", remove it from the world along with its references in inventory
		if(hasItemExpired)
		{
			UE_LOG(LogTemp, Display, TEXT("Item has expired"));
			if(TBackpackHeldItems.Contains(EquippedItem))
			{
				UE_LOG(LogTemp, Display, TEXT("Item in backpack"));
				RemoveItemAtIndex(TBackpackHeldItems.Find(EquippedItem),true,false);
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Item in pockets"));
				RemoveItemAtIndex(TPocketsHeldItems.Find(EquippedItem),false,false);
			}
		}
	}
	return hasItemExpired;
}

int UInventoryComponent::FindEquippedItem()
{
	if(EquippedItem)
	{
		int index = -1;
		index = TPocketsHeldItems.Find(EquippedItem);
		if(index == INDEX_NONE)
		{
			index = TBackpackHeldItems.Find(EquippedItem);
		}
		return (index == INDEX_NONE) ? -1 : index;
	}
	else
	{
		return -1;
	}
}
bool UInventoryComponent::IsEquippedItem(int index, bool isBackpackInventory)
{
	TArray<AUsableObjects*> targetInventory;
	targetInventory = (isBackpackInventory) ? TBackpackHeldItems : TPocketsHeldItems;
	return (targetInventory.IsValidIndex(index) && EquippedItem && targetInventory[index] == EquippedItem);
}
bool UInventoryComponent::DrawEquippedItem() const
{
	return EquippedItem && EquippedItem->ActivateItem();
}
bool UInventoryComponent::SheatheEquippedItem() const
{
	return EquippedItem && EquippedItem->DisableItem();
}
bool UInventoryComponent::AddUpgrade(UUpgrade* newUpgrade)
{
	if(!newUpgrade)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed an invalid upgrade"))
		return false;
	}
	for (UUpgrade* upgrade : TUpgrades)
	{
		if(*upgrade == *newUpgrade)
		{
			return false;
		}
	}
	if(newUpgrade->EnableUpgrade())
	{
		TUpgrades.Emplace(newUpgrade);
		return true;
	}
	else
	{
		return false;
	}
}
bool UInventoryComponent::RemoveUpgrade(int index)
{
	if(TUpgrades.IsValidIndex(index) && TUpgrades[index]->DisableUpgrade())
	{
		UUpgrade* removedUpgrade = TUpgrades[index];
		TUpgrades.RemoveAt(index);
		removedUpgrade->DestroyComponent();
		return true;
	}
	else
	{
		return false;
	}
}