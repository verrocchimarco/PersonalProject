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
    WidgetInventory = Cast<UPlayerMenuWidget>(CreateWidget(this, WidgetInventoryClass,TEXT("Inventory Menu")));
    if(!WidgetInventory)
        UE_LOG(LogTemp, Error, TEXT("WidgetClass component not set!"));
    WidgetInventory->SetVisibility(ESlateVisibility::Collapsed);
	WidgetInventory->AddToViewport();
}
void AProtagonistController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    float playerSpeed = aControlledCharacter->GetVelocity().Length();
    if( playerSpeed > fMinimumVelocityForRotation)
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
void AProtagonistController::ToggleInventory()
{
    if(!bIsInventoryOpen)
    {
        bIsInventoryOpen = true;
        FInputModeGameAndUI fInputMode;
        fInputMode.SetHideCursorDuringCapture(true);
        fInputMode.SetWidgetToFocus(WidgetInventory->TakeWidget());
        Cast<UCharacterMovementComponent>(aControlledCharacter->GetMovementComponent())->DisableMovement();
        SetIgnoreLookInput(true);
        bShowMouseCursor=true;
        bEnableClickEvents=true;
        bEnableMouseOverEvents=true;
        WidgetInventory->RequestInventoryUpdate();
        WidgetInventory->SetVisibility(ESlateVisibility::Visible);
        SetInputMode(fInputMode);
    }
    else
    {
        bIsInventoryOpen = false;
        FInputModeGameOnly fInputMode;
        Cast<UCharacterMovementComponent>(aControlledCharacter->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Walking);
        ResetIgnoreLookInput();
        bShowMouseCursor=false;
        bEnableClickEvents=false;
        bEnableMouseOverEvents=false;
        WidgetInventory->SetVisibility(ESlateVisibility::Collapsed);
        SetInputMode(fInputMode);
    }
}