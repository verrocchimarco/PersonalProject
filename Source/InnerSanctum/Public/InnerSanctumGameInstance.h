// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NotesDataAsset.h"
#include "Containers/Map.h"
#include "InnerSanctumGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class INNERSANCTUM_API UInnerSanctumGameInstance : public UGameInstance
{
	GENERATED_BODY()
	protected:
		UPROPERTY(EditDefaultsOnly)
		UNotesDataAsset* NotesCollection;
		TMap<int32,TPair<bool,bool>> MNotesStatus; // index <found,read>
		TArray<FText> LocationsList;
	public:
		virtual void Init() override;
		// Notes management
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<FNoteData>& GetNotesCollection() const { return NotesCollection->notes; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<FText> GetLocationsList() const { return LocationsList; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<int32> GetFoundNotes() const;
		UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<int32> GetReadFoundNotes() const;
		UFUNCTION(BlueprintCallable)
		bool SetNoteAsFound(int32 index);
		UFUNCTION(BlueprintCallable)
		bool SetNoteAsRead(int32 index);
};
