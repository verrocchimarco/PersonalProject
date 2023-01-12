// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomManager.h"
#include "RoomBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlueprintGameplayTagLibrary.h"
// Sets default values
ARoomManager::ARoomManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoomManager::BeginPlay()
{
	Super::BeginPlay();
	RetrieveSpawnRooms();
	if(bHasRandomPlacement)
	{
		StartupRandomActorPlacement();
	}
}

// Called every frame
void ARoomManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Gets all rooms in the level that allow spawn handling
void ARoomManager::RetrieveSpawnRooms()
{
	FGameplayTagQueryExpression spawnRoomsConditions;
	spawnRoomsConditions.AllExprMatch();
	// Find rooms that allow spawn...
	spawnRoomsConditions.AddExpr(FGameplayTagQueryExpression().AllTagsMatch().AddTag(TAG_RoomAISpawn));
	// ... Except for locked rooms
	spawnRoomsConditions.AddExpr(FGameplayTagQueryExpression().NoTagsMatch().AddTag(TAG_RoomLocked));
	for(FGameplayTagQueryExpression& expr : spawnRoomsConditions.ExprSet)
	{
		UE_LOG(LogTemp, Display, TEXT("Search type: %d"), expr.ExprType);
		for(FGameplayTag& tag: expr.TagSet)
		{
			UE_LOG(LogTemp, Display, TEXT("Search tag: %s"), *(tag.ToString()));
		}
	}
	FGameplayTagQuery querySpawnRooms = FGameplayTagQuery::BuildQuery(spawnRoomsConditions);
	UBlueprintGameplayTagLibrary::GetAllActorsOfClassMatchingTagQuery(GetWorld(),ARoomBox::StaticClass(),querySpawnRooms,SpawnRooms);
	for(AActor* a : SpawnRooms)
	{
		UE_LOG(LogTemp, Display, TEXT("Room Manager: retrieved room %s"), *(Cast<ARoomBox>(a)->GetRoomName()));
	}
}
void ARoomManager::StartupRandomActorPlacement()
{
	FGameplayTag queryTag;
	TArray<AActor*> actorsArray;
	FGameplayTagQuery queryRandomSpawnableActors;
	TArray RoomArrayCopy = SpawnRooms;
	int numberOfRooms, randomRoomIndex;
	ARoomBox* randomRoom;
	AActor* spawningActor;

	queryTag = queryTag.RequestGameplayTag("WorldActors.CanRandomSpawn");
	queryRandomSpawnableActors = FGameplayTagQuery::MakeQuery_MatchTag(queryTag);
	numberOfRooms = SpawnRooms.Num();

	UBlueprintGameplayTagLibrary::GetAllActorsOfClassMatchingTagQuery(GetWorld(),AActor::StaticClass(),queryRandomSpawnableActors,actorsArray);
	while(!actorsArray.IsEmpty())
	{
		if(!numberOfRooms)
		{
			UE_LOG(LogTemp, Warning, TEXT("Room Manager: No rooms available for random spawn"));
			return;
		}
		randomRoomIndex = UKismetMathLibrary::RandomIntegerInRange(0,numberOfRooms-1);
		randomRoom = Cast<ARoomBox>(RoomArrayCopy[randomRoomIndex]);
		spawningActor = actorsArray[0];
		if(randomRoom && spawningActor)
		{
			// If spawn fails, the room is not available, so it's removed
			if(!randomRoom->MoveActorToRoom(spawningActor))
			{
				RoomArrayCopy.RemoveAt(randomRoomIndex);
				--numberOfRooms;
				UE_LOG(LogTemp, Display, TEXT("Room Manager: Failed to place actor %s"),*(spawningActor->GetName()));
			}
			// Otherwise the actor has been moved correctly and can be removed from this cycle
			else
			{
				actorsArray.RemoveAt(0);
				UE_LOG(LogTemp, Display, TEXT("Room Manager: Placed actor %s"),*(spawningActor->GetName()));
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Room Manager: All actors placed"));
	return;
}