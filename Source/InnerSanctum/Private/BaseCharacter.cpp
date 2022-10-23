// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	uHealthComponent     = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	UE_LOG(LogTemp, Display, TEXT("Called base class"));
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Set speed
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABaseCharacter::StartSprint);
    PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABaseCharacter::StopSprint);
    PlayerInputComponent->BindAction("Crouching", EInputEvent::IE_Released, this, &ABaseCharacter::ToggleCrouch);
}

// Movement Bindings

void ABaseCharacter::StartSprint()
{
    UE_LOG(LogTemp, Display, TEXT("Start Sprint"));
    if(GetCharacterMovement()->IsCrouching())
        UnCrouch();
    GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void ABaseCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ABaseCharacter::ToggleCrouch()
{
    // UE_LOG(LogTemp, Display, TEXT("Toggling crouch"));
    if(GetCharacterMovement()->IsCrouching())
    {
        // UE_LOG(LogTemp, Display, TEXT("Uncrouching"));
        UnCrouch();
    }
    else
    {
        StopSprint();
        // UE_LOG(LogTemp, Display, TEXT("Crouching"));
        Crouch();
    }
}