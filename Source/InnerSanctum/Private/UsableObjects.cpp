// Fill out your copyright notice in the Description page of Project Settings.


#include "UsableObjects.h"

// Sets default values
AUsableObjects::AUsableObjects()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ObjectMesh			 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Object Mesh"));
	RootComponent = ObjectMesh;
}

// Called when the game starts or when spawned
void AUsableObjects::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUsableObjects::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AUsableObjects::UseItem()
{
	UE_LOG(LogTemp, Display, TEXT("You used me, I'm %s"),*(GetName()));
	return true;
}