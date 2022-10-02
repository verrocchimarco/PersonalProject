// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UTexture2D;
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Special Item Flag", meta=(DisplayName="Is Key Item"))
    bool bIsKeyItem;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Special Item Flag", meta=(DisplayName="Is Upgrade"))
    bool bIsUpgrade;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Special Item Flag", meta=(DisplayName="Is Weapon"))
    bool bIsWeapon;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Size", meta=(DisplayName="Occupied Slots"))
    int iSlotSize;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
