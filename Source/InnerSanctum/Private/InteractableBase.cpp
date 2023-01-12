// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBase.h"
#include "Materials/MaterialInstanceDynamic.h"
// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EmissiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	RootComponent = EmissiveMesh;
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableBase::SetEmissiveMesh(UStaticMesh* newMesh)
{
	EmissiveMesh->SetStaticMesh(newMesh);
}