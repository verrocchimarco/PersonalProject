// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBox.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnvironmentAffectedInterface.h"
#include "Components/ArrowComponent.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_RoomManagement, 		"RoomManagement");
UE_DEFINE_GAMEPLAY_TAG(TAG_AffectsActors, 		"RoomManagement.AffectsActors");
UE_DEFINE_GAMEPLAY_TAG(TAG_Pathfindable, 		"RoomManagement.PathfindingSelectable");
UE_DEFINE_GAMEPLAY_TAG(TAG_SpawnManagerHandler,	"RoomManagement.SpawnManager");
UE_DEFINE_GAMEPLAY_TAG(TAG_RoomAISpawn,			"RoomManagement.SpawnManager.AllowsAISpawn");
UE_DEFINE_GAMEPLAY_TAG(TAG_RoomLocked,			"RoomManagement.SpawnManager.Locked");

// Sets default values
ARoomBox::ARoomBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bGenerateOverlapEventsDuringLevelStreaming = true;
}

 void ARoomBox::OnConstruction (const FTransform& Transform)
 {
	Super::OnConstruction(Transform);
	// Only proceed with construction routine if the ShapeType has been changed
	if(RoomCollisionAreaShape && RoomCollisionAreaShape->GetCollisionShape().ShapeType == RoomShape.GetValue())
	{
		UE_LOG(LogTemp, Display, TEXT("RoomBox: Room Area Shape is %d and matches RoomShape param %d"), RoomCollisionAreaShape->GetCollisionShape().ShapeType, RoomShape.GetValue());
		return;
	}
	if(RoomCollisionAreaShape)
	{
		RoomCollisionAreaShape->DestroyComponent();
	}
	switch(RoomShape)
	{
		case(ERoomShape::Box):
			RoomCollisionAreaShape = Cast<UBoxComponent>(NewObject<UBoxComponent>(this));
		break;
		case(ERoomShape::Sphere):
			RoomCollisionAreaShape = Cast<USphereComponent>(NewObject<USphereComponent>(this));
		break;
		case(ERoomShape::Capsule):
			RoomCollisionAreaShape = Cast<UCapsuleComponent>(NewObject<UCapsuleComponent>(this));
		break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("RoomBox: No shape selected for Room %s. Defaulting to Box"),*(RoomName));
			RoomShape = ERoomShape::Box;
			RoomCollisionAreaShape = Cast<UBoxComponent>(NewObject<UBoxComponent>(this));
		break;
	}
	RoomCollisionAreaShape->SetupAttachment(RootComponent);
 }
// Called when the game starts or when spawned
void ARoomBox::BeginPlay()
{
	Super::BeginPlay();
	PrintGameplayTags();
	// Prevents the creation of a room without the RoomManagement tag
	check(TagSpawnManager.HasTag(TAG_RoomManagement));
	// Prevents the creation of a room without a collision area
	check(RoomCollisionAreaShape);
	if(TagSpawnManager.HasTag(TAG_AffectsActors))
	{
		RoomCollisionAreaShape->OnComponentBeginOverlap.AddDynamic(this, &ARoomBox::BeginOverlap);
		RoomCollisionAreaShape->OnComponentEndOverlap.AddDynamic(this, &ARoomBox::EndOverlap);
	}
}

// Called every frame
void ARoomBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomBox::PrintGameplayTags()
{
	UE_LOG(LogTemp, Display, TEXT("RoomBox: %s. Tags: %s"), *(RoomName), *(TagSpawnManager.ToStringSimple()));
}

void ARoomBox::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("RoomBox: Overlap detected. Room: %s, Actor: %s"), *(RoomName),*(OtherActor->GetName()));
	if(OtherActor->Implements<UEnvironmentAffectedInterface>())
	{
		IEnvironmentAffectedInterface::Execute_SetRoomEntrance(OtherActor,RoomSize.GetValue());	
	}
}

void ARoomBox::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Display, TEXT("RoomBox: Overlap detected. Room: %s, Actor: %s"), *(RoomName),*(OtherActor->GetName()));
	if(OtherActor->Implements<UEnvironmentAffectedInterface>())
	{
		IEnvironmentAffectedInterface::Execute_SetRoomExit(OtherActor,RoomSize.GetValue());	
	}
}

void ARoomBox::ChangeLockedStatus(bool bIsLocked)
{
	if(bIsLocked && TagSpawnManager.HasTag(TAG_SpawnManagerHandler))
	{
		TagSpawnManager.AddTag(TAG_RoomLocked);
	}
	else
	{
		TagSpawnManager.RemoveTag(TAG_RoomLocked);
	}
}
void ARoomBox::ChangeAISpawnStatus(bool bAllowsSpawn)
{
	if(bAllowsSpawn && TagSpawnManager.HasTag(TAG_SpawnManagerHandler))
	{
		TagSpawnManager.AddTag(TAG_RoomAISpawn);
	}
	else
	{
		TagSpawnManager.RemoveTag(TAG_RoomAISpawn);
	}
}

void ARoomBox::ChangeAffectsActors(bool bAffectsActors)
{
	if(bAffectsActors)
	{
		TagSpawnManager.AddTag(TAG_AffectsActors);
		RoomCollisionAreaShape->OnComponentBeginOverlap.AddDynamic(this, &ARoomBox::BeginOverlap);
	}
	else
	{
		TagSpawnManager.RemoveTag(TAG_AffectsActors);
		RoomCollisionAreaShape->OnComponentBeginOverlap.RemoveDynamic(this, &ARoomBox::BeginOverlap);
	}
}
void ARoomBox::ChangePathfindingSelectable(bool bIsPathfindingSelectable)
{
	if(bIsPathfindingSelectable)
	{
		TagSpawnManager.AddTag(TAG_Pathfindable);
	}
	else
	{
		TagSpawnManager.RemoveTag(TAG_Pathfindable);
	}
}

bool ARoomBox::MoveActorToRoom(AActor* actor)
{
	if(!AcceptNewSpawns())
	{
		return false;
	}
	++currentlySpawnedActors;
	actor->SetActorLocation(FMath::RandPointInBox(GetComponentsBoundingBox()),false,nullptr,ETeleportType::ResetPhysics);
	return true;
}