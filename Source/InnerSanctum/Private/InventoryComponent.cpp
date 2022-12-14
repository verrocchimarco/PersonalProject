// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "ProtagonistCharacter.h"
#include "Kismet/KismetArrayLibrary.h"
#include "InnerSanctumGameInstance.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	TPocketsHeldItems.Reserve(PocketsInventorySize);
	TBackpackHeldItems.Reserve(BackpackInventorySize);
	fAvailablePocketsSpace = PocketsInventorySize;
	fAvailableBackpackSpace = BackpackInventorySize;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	// Setup basic backpack mesh onto the character
	CharacterBackpackMesh = Cast<AProtagonistCharacter>(GetOwner())->GetBackpackMesh();
	SetBackpackMesh(BackpackMesh);
	GameInstanceInit();
	if(bHasBackpack)
	{
		EnableBackpack();
	}
	else
	{
		RemoveBackpack();
	}
	UE_LOG(LogTemp, Display, TEXT("Inventory Component: Available space in pockets: %d"),fAvailablePocketsSpace);
	UE_LOG(LogTemp, Display, TEXT("Inventory Component: Available space in backpack: %d"),fAvailableBackpackSpace);
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
	if(newSize > BackpackInventorySize)
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Increasing backpack size"))
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
	if(!newItem)
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Item added does not exist"));
		return false;
	}
	newItem->SetActorHiddenInGame(true);
	int itemSlotSize = newItem->GetInventoryDetails().iSlotSize;
	if( fAvailablePocketsSpace - itemSlotSize >= 0 )
	{
		TPocketsHeldItems.Emplace(newItem);
		fAvailablePocketsSpace -= itemSlotSize;
	}
	else if ( fAvailableBackpackSpace - itemSlotSize >= 0 && bHasBackpack)
	{
		TBackpackHeldItems.Emplace(newItem);
		fAvailableBackpackSpace -= itemSlotSize;
	}
	else
		return false;
	return true;
}

bool UInventoryComponent::RemoveItem(int index, bool isBackpackInventory, bool shouldSpawnPickup)
{
	bool removalSuccessful = false;
	AUsableObjects* ObjectReference = nullptr;
	UE_LOG(LogTemp, Display, TEXT("Inventory Component: Remove index: %d"),index);
	if(!isBackpackInventory && TPocketsHeldItems.IsValidIndex(index))
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Item in pockets removal"));
		fAvailablePocketsSpace += TPocketsHeldItems[index]->GetInventoryDetails().iSlotSize;
		removalSuccessful = true;
		ObjectReference = TPocketsHeldItems[index];
		TPocketsHeldItems.RemoveAtSwap(index);
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Item in backpack removal"));
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
			UE_LOG(LogTemp, Display, TEXT("Inventory Component: Unequipping item"));
			UnequipItem();
		}
		if(shouldSpawnPickup)
		{
			// Spawn PickupActor
			FVector spawningLocation = GetOwner()->GetActorLocation();
			FActorSpawnParameters spawnParameters;
			spawningLocation.X += 25;
			spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			GetWorld()->SpawnActor(ObjectReference->GetPickupClassReference(),&spawningLocation,0);
		}
		ObjectReference->Destroy();
	}
	return removalSuccessful;
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
			UE_LOG(LogTemp, Display, TEXT("Inventory Component: Removing Item at %d, available pocket space %d"),index,fAvailablePocketsSpace);
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
			UE_LOG(LogTemp, Display, TEXT("Inventory Component: Removing Item at %d, available backpack space %d"),index,fAvailableBackpackSpace);
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
	UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swaprequested.\nItem 1 index:[%d] isBackpack[%d]\nItem 2 index:[%d] isBackpack[%d]"),firstItemIndex,logb1,secondItemIndex,logb2);
	// Return true if the items are in the same sub inventory. Switch useless, but perform it because otherwise it looks bad on the UI. Goddamn gamers
	if( (isFirstItemInBackpack && isSecondItemInBackpack) )
	{
		if(firstItemIndex >= 0 && secondItemIndex >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapnot needed. Both in backpack"));
			TBackpackHeldItems.Swap(firstItemIndex,secondItemIndex);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Both in backpack but one doesn't exist"));
			return false;
		}
	}
	if ( (!isFirstItemInBackpack && !isSecondItemInBackpack))
	{
		if(firstItemIndex >= 0 && secondItemIndex >= 0 )
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapnot needed. Both in pockets"));
			TPocketsHeldItems.Swap(firstItemIndex,secondItemIndex);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Both in items but one doesn't exist"));
			return false;
		}
	}
	// Stop swap if both indexes are -1
	if (firstItemIndex < 0 && secondItemIndex < 0 )
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Both don't exist"));
		return false;
	}
	// Don't proceed if one of the items is in the backpack when the backpack isn't available
	if( (isFirstItemInBackpack || isSecondItemInBackpack) && !bHasBackpack )
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Backpack not available"));
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
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Available size not sufficient. Pockets projected size [%d] Backpack projected size [%d]"), swapSizeResultPockets,swapSizeResultBackpack);
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
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Available size not sufficient. Backpack projected size [%d]"),swapSizeResultBackpack);
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
			UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Available size not sufficient. Pockets projected size [%d]"),swapSizeResultPockets);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component:  Swapimpossible. Both items don't exist"));
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
		EquippedItemLocation.Key = index;
		EquippedItemLocation.Value = isBackpackInventory;
	}
	else if(isBackpackInventory && bHasBackpack && TBackpackHeldItems.IsValidIndex(index))
	{
		newEquippedItem = TBackpackHeldItems[index];
		equipSuccessful = true;
		EquippedItemLocation.Key = index;
		EquippedItemLocation.Value = isBackpackInventory;
	}
	// If previously equipped item was being held, play switch animation
	if(equipSuccessful && newEquippedItem && EquippedItem && bIsEquippedItemDrawn)
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
	if(EquippedItem)
	{
		EquippedItem->DisableItem();
		bIsEquippedItemDrawn = false;
		EquippedItemLocation.Key = -1;
		EquippedItemLocation.Value = false;
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
			int equippedItemIndex = EquippedItemLocation.Key;
			bool equippedItemIsBackpack = EquippedItemLocation.Value;
			UE_LOG(LogTemp, Display, TEXT("Item has expired"));
			RemoveItem(equippedItemIndex,equippedItemIsBackpack,false);
			bIsEquippedItemDrawn = false;
		}
	}
	return hasItemExpired;
}

bool UInventoryComponent::IsEquippedItem(int index, bool isBackpackInventory)
{
	return EquippedItemLocation.Key == index && EquippedItemLocation.Value == isBackpackInventory;
}
void UInventoryComponent::ToggleDrawEquippedItem()
{
	if(EquippedItem)
	{
		if(bIsEquippedItemDrawn)
		{
			bIsEquippedItemDrawn = false;
			EquippedItem->DisableItem();
		}
		else
		{
			bIsEquippedItemDrawn = true;
			EquippedItem->ActivateItem();
		}
	}
}

bool UInventoryComponent::AddUpgrade(UUpgrade* newUpgrade)
{
	if(!newUpgrade)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed an invalid upgrade"))
		return false;
	}
	// Do not allow duplicates
	for (UUpgrade* upgrade : TUpgrades)
	{
		if(*upgrade == *newUpgrade && upgrade->GetUpgradeType() == UpgradeType::Generic)
		{
			return false;
		}
	}
	if(newUpgrade->EnableUpgrade())
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Upgrade enabled. Adding it to array"))
		// If the new upgrade is of type Backpack but the character currently doesn't have its backpack, remove any pre existing Backpack upgrade and replace it
		int32 preExistingBackpackIndex = TUpgrades.IndexOfByPredicate([&](UUpgrade* const upgrade) { return upgrade && upgrade->GetUpgradeType() == UpgradeType::Backpack; });
		if(preExistingBackpackIndex != INDEX_NONE && !bHasBackpack)
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component: Replacing pre existing backpack"))
			TUpgrades.RemoveAtSwap(preExistingBackpackIndex);
		}
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
		return true;
	}
	else
	{
		return false;
	}
}

void UInventoryComponent::EnableBackpack()
{
	UE_LOG(LogTemp, Display, TEXT("Inventory Component: Enabling Backpack"))
	CharacterBackpackMesh->SetVisibility(true);
	bHasBackpack=true;
}

void UInventoryComponent::RemoveBackpack()
{
	UE_LOG(LogTemp, Display, TEXT("Inventory Component: Disabling backpack"))
	CharacterBackpackMesh->SetVisibility(false);
	bHasBackpack = false;
}

void UInventoryComponent::SetBackpackMesh(UStaticMesh* newBackpackMesh)
{
	BackpackMesh = newBackpackMesh;
	if(BackpackMesh)
	{
		CharacterBackpackMesh->SetStaticMesh(newBackpackMesh);
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Setting backpack mesh [%s]"),*(BackpackMesh->GetName()));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: No backpack mesh defined"));
	}
}	
void UInventoryComponent::GameInstanceInit()
{
	UInnerSanctumGameInstance* gameInstance = Cast<UInnerSanctumGameInstance>(GetWorld()->GetGameInstance());
	UE_LOG(LogTemp, Display, TEXT("Inventory Component: retrieving data from game instance"));
	AProtagonistCharacter* playerCharacter = Cast<AProtagonistCharacter>(GetOwner());
	// Always call Upgrades first to ensure all the statuses upgrades are applied before the inventory might be changed
	for(TSubclassOf<UUpgrade> upgradeClass : gameInstance->GetPlayerUpgrades())
	{
		UUpgrade* newUpgrade = NewObject<UUpgrade>(this,upgradeClass);
		newUpgrade->SetMainCharacter(playerCharacter);
		AddUpgrade(newUpgrade);
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Adding upgrade: [%s]"),*(newUpgrade->GetName()));
	}
	for(TSubclassOf<AUsableObjects> itemClass : gameInstance->GetPlayerPocketsItems())
	{
		AUsableObjects* item = Cast<AUsableObjects>(GetWorld()->SpawnActor(itemClass));
		TPocketsHeldItems.Emplace(item);
		fAvailablePocketsSpace -= item->GetInventoryDetails().iSlotSize;
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Adding pocket item: [%s]"),*(item->GetName()));	
	}
	for(TSubclassOf<AUsableObjects> itemClass : gameInstance->GetPlayerBackpackItems())
	{
		AUsableObjects* item = Cast<AUsableObjects>(GetWorld()->SpawnActor(itemClass));
		TBackpackHeldItems.Emplace(item);
		fAvailableBackpackSpace -= item->GetInventoryDetails().iSlotSize;
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Adding backpack item: [%s]"),*(item->GetName()));
	}
	EquippedItemLocation 	= gameInstance->GetEquippedItemLocation(); UE_LOG(LogTemp, Display, TEXT("Equipped Item location:[%d][%d]"),EquippedItemLocation.Key,EquippedItemLocation.Value);
	bHasBackpack 			= gameInstance->GetPlayerHasBackpack(); UE_LOG(LogTemp, Display, TEXT("Player backpack availability:[%d]"),bHasBackpack);
	// Equip item if it's in an available location
	if(EquippedItemLocation.Key != -1)
	{
		UE_LOG(LogTemp, Display, TEXT("Inventory Component: Equipped item location is valid"));
		// If it's in the backpack but the backpack is not available, unset the equipped item location
		if(EquippedItemLocation.Value && !bHasBackpack)
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component: Equipped item points at backpack"));
			EquippedItemLocation = TPair<int,bool>(-1,false);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Inventory Component: setting equipped item to index %d"),EquippedItemLocation.Key);
			EquipItem(EquippedItemLocation.Key, EquippedItemLocation.Value);
		}
	}
}