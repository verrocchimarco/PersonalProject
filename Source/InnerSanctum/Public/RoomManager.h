// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentAffectedInterface.h"
#include "RoomManager.generated.h"

class ARoomBox;
UCLASS()
class INNERSANCTUM_API ARoomManager : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Activate Enemies Random Placement"))
	bool bHasRandomPlacement;
	UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Spawn Extra Enemies"))
	bool bAllowsNewSpawns;
	UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Allowed Random Enemies and Spawn chance", EditCondition="bAllowsNewSpawns"))
	TMap<TSubclassOf<AActor>,uint8> mRandomEnemies;
	UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Max Number of Random Enemies", EditCondition="bAllowsNewSpawns"))
	uint8 iMaxRandEnemies = 0;
	uint8 iCurrentRandEnemies = 0;
	UPROPERTY(EditAnywhere, Category = "Room Effect", meta=(DisplayName="Default Level Size"))
	TEnumAsByte<EAreaSize> LevelSize;
	TArray<AActor*> SpawnRooms;

	void RetrieveSpawnRooms();
	void StartupRandomActorPlacement();
public:	
	// Sets default values for this actor's properties
	ARoomManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
