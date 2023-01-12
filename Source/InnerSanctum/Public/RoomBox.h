// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GameplayTagAssetInterface.h"
#include "CollisionShape.h"
#include "Components/ShapeComponent.h"
#include "EnvironmentAffectedInterface.h"
#include "RoomBox.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_RoomManagement);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AffectsActors);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Pathfindable);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SpawnManagerHandler);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_RoomAISpawn);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_RoomLocked);

UENUM()
enum ERoomShape
{
	Box		= 1,
	Sphere	= 2,
	Capsule	= 3
};

class UTextRenderComponent;
class UArrowComponent;
UCLASS()
class INNERSANCTUM_API ARoomBox : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

private:
	// Add arrow component for specific respawn points
	UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Category = "Room Area", meta=(AllowPrivateAccess="true"))
	UShapeComponent* RoomCollisionAreaShape;
	UPROPERTY(EditAnywhere, Category = "Room Area")
	TEnumAsByte<ERoomShape> RoomShape = ERoomShape::Box;
	UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Has Limited Spawnable Actors"))
	bool bLimitedSpawn = false;
	UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(EditCondition="bLimitedSpawn"))
	uint8 maxSpawnableActors = 0;
	uint8 currentlySpawnedActors = 0;
	// UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Use Fixed Spawn Points"))
	// bool bHasFixedSpawnPoints = false;
	// UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Use Fixed Spawn Only", EditCondition="bHasFixedSpawnPoints"))
	// bool bFixedSpawnPointsOnly = false;
	// UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Objects Spawn Points", EditCondition="bHasFixedSpawnPoints"))
	// TArray<UArrowComponent*> tObjectsSpawnPoints;
	// UPROPERTY(EditAnywhere, Category = "Room Spawn", meta=(DisplayName="Characters Spawn Points", EditCondition="bHasFixedSpawnPoints"))
	// TArray<UArrowComponent*> tCharactersSpawnPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags", meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer TagSpawnManager;
	UPROPERTY(EditAnywhere, Category = "Debug")
	FString RoomName;
	UPROPERTY(EditAnywhere, Category = "Room Effect", meta=(DisplayName="Room Size"))
	TEnumAsByte<EAreaSize> RoomSize;

	bool AcceptNewSpawns() const { return (!bLimitedSpawn || (bLimitedSpawn && currentlySpawnedActors < maxSpawnableActors)); }
public:	
	// Sets default values for this actor's properties
	ARoomBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction (const FTransform& Transform) override;
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = TagSpawnManager; return; }

	// DEBUG
	void PrintGameplayTags();

	// Tag Methods
	void ChangeLockedStatus(bool bIsLocked);
	void ChangeAISpawnStatus(bool bAllowsSpawn);
	void ChangeAffectsActors(bool bAffectsActors);
	void ChangePathfindingSelectable(bool bIsPathfindingSelectable);

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UShapeComponent* GetRoomCollisionAreaShape() const { return RoomCollisionAreaShape; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetRoomName() const { return RoomName; }

	//Spawn
	UFUNCTION(BlueprintCallable)
	bool MoveActorToRoom(AActor* actor);
};
