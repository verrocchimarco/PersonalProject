// Fill out your copyright notice in the Description page of Project Settings.


#include "InnerSanctumGameInstance.h"
#include "NotesDataAsset.h"
void UInnerSanctumGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Display, TEXT("Game Instance started. Good boy"));
    int32 index = 0;
    for(auto& noteAsset : NotesCollection->notes)
    {
        MNotesStatus.Add(index++, TPair<bool,bool>(false,false));
        if(!LocationsList.ContainsByPredicate([&](const FText location){ return !location.CompareToCaseIgnored(noteAsset.Location); }))
        {
            LocationsList.Emplace(noteAsset.Location);
        }
    }
}

TArray<int32> UInnerSanctumGameInstance::GetFoundNotes() const
{
    TArray<int32> result;
    TArray<TPair<bool,bool>> MapNotesStatusValues;
    MNotesStatus.GenerateValueArray(MapNotesStatusValues);
    int index = 0;
    for(const TPair<bool,bool>& status : MapNotesStatusValues)
    {
        if(status.Key)
        {
            result.Emplace(index);
        }
        index++;
    }
    return result;
}

TArray<int32> UInnerSanctumGameInstance::GetReadFoundNotes() const
{
    TArray<int32> result;
    TArray<TPair<bool,bool>> MapNotesStatusValues;
    MNotesStatus.GenerateValueArray(MapNotesStatusValues);
    int index = 0;
    for(const TPair<bool,bool>& status : MapNotesStatusValues)
    {
        if(status.Key && status.Value)
        {
            result.Emplace(index);
        }
        index++;
    }
    return result;
}

bool UInnerSanctumGameInstance::SetNoteAsFound(int32 index)
{
    if(MNotesStatus.Contains(index))
    {
        MNotesStatus[index].Key = true;
        return true;
    }
    else
    {
        return false;
    }
}

bool UInnerSanctumGameInstance::SetNoteAsRead(int32 index)
{
    if(MNotesStatus.Contains(index))
    {
        MNotesStatus[index].Value = true;
        return true;
    }
    else
    {
        return false;
    }
}