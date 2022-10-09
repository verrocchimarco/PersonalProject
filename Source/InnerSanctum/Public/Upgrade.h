// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UsableObjects.h"
#include "Upgrade.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UUpgrade : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpgrade();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Upgrade Description", meta=(DisplayName="Inventory Details"))
	FInventoryItem sInventoryDetails;
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	bool EnableUpgrade();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	bool DisableUpgrade();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FInventoryItem GetUpgradeInventoryDetails() const { return sInventoryDetails; }
		
};

FORCEINLINE bool operator==(const UUpgrade& lhs, const UUpgrade& rhs)
{
	return lhs.GetUpgradeInventoryDetails().tItemName.CompareTo(rhs.GetUpgradeInventoryDetails().tItemName) == 0;
}
FORCEINLINE bool operator!=(const UUpgrade& lhs, const UUpgrade& rhs) { return !operator==(lhs,rhs); }