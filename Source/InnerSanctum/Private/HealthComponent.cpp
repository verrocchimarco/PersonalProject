// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "ProtagonistCharacter.h"
#include "Kismet/GameplayStatics.h"
// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter 				= Cast<AProtagonistCharacter>(GetOwner());
	fCurrentHealth 				= fMaxHealth;
	fHeadCurrentHealth 			= fHeadMaxHealth;
	fUpperLimbsCurrentHealth	= fUpperLimbsMaxHealth;
	fLowerLimbsCurrentHealth	= fLowerLimbsMaxHealth;

	OwnerCharacter->OnTakePointDamage.AddDynamic(this, &UHealthComponent::OnTakePointDamage);
	UE_LOG(LogTemp, Display, TEXT("Your message"));
}
// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser )
{
	fCurrentHealth = FMath::Clamp( fCurrentHealth-Damage, 0.f, fMaxHealth);
	UE_LOG(LogTemp, Display, TEXT("Current Health: %f"), fCurrentHealth);
}