// Fill out your copyright notice in the Description page of Project Settings.


#include "UsableObjects.h"
#include "Kismet/GameplayStatics.h"
#include "ProtagonistCharacter.h"

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
	playerCharacter = Cast<AProtagonistCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if(playerCharacter)
	{
		AttachmentSocket = playerCharacter->GetItemBoneSocket();
		CharacterMesh = playerCharacter->GetMesh();
	}
}

// Called every frame
void AUsableObjects::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AUsableObjects::ActivateItem()
{
	FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget,true);
	AttachToComponent(CharacterMesh,transformRules,FName(*AttachmentSocket));
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	bIsActive = true;
	return true;
}
bool AUsableObjects::DisableItem()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsActive = false;
	return true;
}