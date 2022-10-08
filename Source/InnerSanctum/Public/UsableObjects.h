// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UsableObjects.generated.h"

class AInteractableBase;
UENUM(BlueprintType)
enum ItemCategory
{
	HealingItem     UMETA(DisplayName = "Healing Item"),
	Weapon          UMETA(DisplayName = "Weapon"),
    KeyItem         UMETA(DisplayName = "Key Item"),
    UpgradeItem     UMETA(DisplayName = "Upgrade"),
    GenericItem     UMETA(DisplayName = "Generic Item"),
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

    //~ The following member variable will be accessible by Blueprint Graphs:
    // This is the tooltip for our test variable.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic Description", meta=(DisplayName="Item Name"))
    FText tItemName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic Description", meta=(DisplayName="Item Description"))
    FText tItemDescription;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic Description", meta=(DisplayName="Item Picture"))
    UTexture2D* texItemPicture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Type", meta=(DisplayName="Item Type"))
    TEnumAsByte<ItemCategory> sItemType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Size", meta=(DisplayName="Occupied Slots"))
    int iSlotSize;
};
 
UCLASS(Abstract, BlueprintType,Blueprintable)
class INNERSANCTUM_API AUsableObjects : public AActor
{
	GENERATED_BODY()
	public:	
		// Sets default values for this actor's properties
		AUsableObjects();

	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Description", meta=(DisplayName="Inventory Details"))
		FInventoryItem sInventoryDetails;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Description", meta=(DisplayName="Inventory Pickup Class"))
		TSubclassOf<AInteractableBase> aPickupClass;
        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* ObjectMesh;
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;
		// Getters
        UFUNCTION(BlueprintCallable)
		FInventoryItem GetInventoryDetails() const { return sInventoryDetails; }
        UFUNCTION(BlueprintCallable)
        TSubclassOf<AInteractableBase> GetPickupClassReference() const { return aPickupClass; }
        virtual bool UseItem();

};
