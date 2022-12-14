// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UsableObjects.h"
#include "Upgrade.generated.h"

UENUM(BlueprintType)
enum UpgradeType
{
	Backpack	UMETA(DisplayName = "Backpack"),
	Generic     UMETA(DisplayName = "Generic"),
};
class AProtagonistCharacter;
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UUpgrade : public UObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpgrade();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Upgrade Description", meta=(DisplayName="Inventory Details"))
	FInventoryItem sInventoryDetails;
	UPROPERTY(BlueprintReadOnly)
	AProtagonistCharacter* mainCharacter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Upgrade Description", meta=(DisplayName="Upgrade type"))
	TEnumAsByte<UpgradeType> sUpgradeType;
public:	
	UFUNCTION(BlueprintCallable)
	void SetMainCharacter(AProtagonistCharacter* playerRef) { mainCharacter = playerRef; }
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	bool EnableUpgrade();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	bool DisableUpgrade();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FInventoryItem GetUpgradeInventoryDetails() const { return sInventoryDetails; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UpgradeType GetUpgradeType() const { return sUpgradeType; }
		
};

FORCEINLINE bool operator==(const UUpgrade& lhs, const UUpgrade& rhs)
{
	return lhs.GetUpgradeInventoryDetails().tItemName.CompareTo(rhs.GetUpgradeInventoryDetails().tItemName) == 0;
}
FORCEINLINE bool operator!=(const UUpgrade& lhs, const UUpgrade& rhs) { return !operator==(lhs,rhs); }