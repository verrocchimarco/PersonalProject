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
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Set speed
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
    uHealthComponent->OnDismembermentEvent.AddDynamic(this, &ABaseCharacter::SpawnDismemberedMesh);
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
    bIsRunning = true;
}

void ABaseCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    bIsRunning = false;
}

void ABaseCharacter::ToggleCrouch()
{
    UE_LOG(LogTemp, Display, TEXT("Base Character: Toggling crouch"));
    if(GetCharacterMovement()->IsCrouching())
    {
        UE_LOG(LogTemp, Display, TEXT("Base Character: Uncrouching"));
        UnCrouch();
    }
    else
    {
        StopSprint();
        UE_LOG(LogTemp, Display, TEXT("Base Character: Crouching"));
        Crouch();
    }
}

// void ABaseCharacter::BeginResting()
// {
//     UE_LOG(LogTemp, Display, TEXT("Base Character: Begin resting"));
//     bIsResting = true;
//     GetWorldTimerManager().SetTimer(tHealthRecoveryTimer,this,&ABaseCharacter::HealWhileResting,fHealthRecoveryDelay,true);
// }
// void ABaseCharacter::HealWhileResting()
// {
//     uHealthComponent->HealHealth(fHealthRecoveryWhileResting);
// }
// void ABaseCharacter::StopResting()
// {
//     UE_LOG(LogTemp, Display, TEXT("Base Character: Stop resting"));
//     bIsResting = false;
//     GetWorld()->GetTimerManager().ClearTimer(tHealthRecoveryTimer);
// }