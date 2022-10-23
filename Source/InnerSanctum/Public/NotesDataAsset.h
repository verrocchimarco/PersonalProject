// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "NotesDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNoteData
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NoteTitle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NoteContent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BackgroundTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Location;
};
UCLASS()
class INNERSANCTUM_API UNotesDataAsset : public UDataAsset
{
	GENERATED_BODY()
	public:
		UPROPERTY(EditAnywhere)
		TArray<FNoteData> notes;		
};