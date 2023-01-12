// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtagonistController.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "ProtagonistCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "PlayerMenuWidget.h"
#include "GameFramework/PlayerStart.h"

void AProtagonistController::BeginPlay()
{
    Super::BeginPlay();
    aControlledCharacter = Cast<AProtagonistCharacter>(GetCharacter());
    if(!aControlledCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter actor not set!"));
        UKismetSystemLibrary::QuitGame(GetWorld(),this,EQuitPreference::Quit,true);
    }
    uPlayerCamera = aControlledCharacter->GetCamera();
    if(!uPlayerCamera)
    {   
        UE_LOG(LogTemp, Error, TEXT("PlayerCamera component not set!"));
        UKismetSystemLibrary::QuitGame(GetWorld(),this,EQuitPreference::Quit,true);
    }
    WidgetPlayerMenu = Cast<UPlayerMenuWidget>(CreateWidget(this, WidgetPlayerMenuClass,TEXT("Inventory Menu")));
    if(!WidgetPlayerMenu)
        UE_LOG(LogTemp, Error, TEXT("WidgetClass component not set!"));
    WidgetPlayerMenu->SetVisibility(ESlateVisibility::Collapsed);
	WidgetPlayerMenu->AddToViewport();
    fDefaultFOV = PlayerCameraManager->DefaultFOV;
    UE_LOG(LogTemp, Display, TEXT("Protagonist Controller: Spawning at: %s"),*(Cast<APlayerStart>(StartSpot)->PlayerStartTag.ToString()));
    
}
void AProtagonistController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    float playerSpeed = aControlledCharacter->GetVelocity().Length();
    if( playerSpeed > fMinimumVelocityForRotation || bIsAiming)
        CameraRotationInterp(DeltaSeconds, playerSpeed);
}

void AProtagonistController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAxis(TEXT("MoveForward"),   this,   &AProtagonistController::MoveForward);
    InputComponent->BindAxis(TEXT("StrafeRight"),   this,   &AProtagonistController::StrafeRight);
    InputComponent->BindAxis(TEXT("LookUp"),        this,   &AProtagonistController::LookUp);
    InputComponent->BindAxis(TEXT("LookRight"),     this,   &AProtagonistController::LookRight);
    InputComponent->BindAction(TEXT("Inventory"), EInputEvent::IE_Pressed, this, &AProtagonistController::ToggleInventory);
    InputComponent->BindAction(TEXT("ReadyUpItem"), EInputEvent::IE_Pressed, this, &AProtagonistController::Aim);
    InputComponent->BindAction(TEXT("ReadyUpItem"), EInputEvent::IE_Released, this, &AProtagonistController::StopAim);
}

void AProtagonistController::MoveForward(float axisValue)
{
    FVector playerForwardVector = aControlledCharacter->GetActorForwardVector();
    aControlledCharacter->AddMovementInput(playerForwardVector,axisValue);
}
void AProtagonistController::StrafeRight(float axisValue)
{
    FVector cameraRightVector = uPlayerCamera->GetRightVector();
    aControlledCharacter->AddMovementInput(cameraRightVector,axisValue);
}

void AProtagonistController::LookUp(float axisValue)
{
    aControlledCharacter->AddControllerPitchInput(fVerticalSensitivity*axisValue*GetWorld()->GetDeltaSeconds());
}
void AProtagonistController::LookRight(float axisValue)
{
    aControlledCharacter->AddControllerYawInput(fHorizontalSensitivity*axisValue*GetWorld()->GetDeltaSeconds());
}

void AProtagonistController::CameraRotationInterp(const float& DeltaSeconds, float& playerSpeed)
{
    FRotator playerRotation, playerDesiredRotation, playerStepRotation;
    FRotator cameraForwardRotation;
    playerRotation = aControlledCharacter->GetActorRotation();
    playerDesiredRotation = playerRotation;
    playerDesiredRotation.Yaw = uPlayerCamera->GetComponentRotation().Yaw;
    playerStepRotation = FMath::RInterpConstantTo(playerRotation, playerDesiredRotation, DeltaSeconds, fCameraBasedRotationSpeed + playerSpeed*0.4f);
    aControlledCharacter->SetActorRotation(playerStepRotation);
}
void AProtagonistController::Aim()
{
    bIsAiming = true;
    aControlledCharacter->SetIsEquipReady(true);
    PlayerCameraManager->SetFOV(fAimFOV);
}
void AProtagonistController::StopAim()
{
    bIsAiming = false;
    aControlledCharacter->SetIsEquipReady(false);
    PlayerCameraManager->UnlockFOV();
}

void AProtagonistController::ToggleInventory()
{
    if(!bIsInventoryOpen && !Cast<UCharacterMovementComponent>(aControlledCharacter->GetMovementComponent())->IsFalling() && !aControlledCharacter->GetIsResting())
    {
        bIsInventoryOpen = true;
        FInputModeUIOnly fInputMode;
        fInputMode.SetWidgetToFocus(WidgetPlayerMenu->TakeWidget());
        Cast<UCharacterMovementComponent>(aControlledCharacter->GetMovementComponent())->DisableMovement();
        bShowMouseCursor=true;
        bEnableClickEvents=true;
        bEnableMouseOverEvents=true;
        WidgetPlayerMenu->RequestInventoryUpdate();
        WidgetPlayerMenu->SetVisibility(ESlateVisibility::Visible);
        SetInputMode(fInputMode);
    }
    else
    {
        bIsInventoryOpen = false;
        FInputModeGameOnly fInputMode;
        Cast<UCharacterMovementComponent>(aControlledCharacter->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Walking);
        bShowMouseCursor=false;
        bEnableClickEvents=false;
        bEnableMouseOverEvents=false;
        WidgetPlayerMenu->SetVisibility(ESlateVisibility::Collapsed);
        aControlledCharacter->EnableInput(this);
        SetInputMode(fInputMode);
    }
}



/*
A better camera manger

void AMyPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	class AMyCharacter * Character = PCOwner ? Cast<AMyCharacter>(PCOwner->GetPawn()) : NULL;

	if (Character != NULL)
	{
		bool IsAiming = Character->IsAiming();

		const float TargetFOV = IsAiming ? 60.0f : 90.0f;

		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%f"), DefaultFOV));

	Super::UpdateCamera(DeltaTime);
}
*/