// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProtagonistController.generated.h"

/**
 * 
 */
class AProtagonistCharacter;
class UCameraComponent;
class UPlayerMenuWidget;
class UUserWidget;
UCLASS()
class INNERSANCTUM_API AProtagonistController : public APlayerController
{
	GENERATED_BODY()
	private:
		UPROPERTY()
		AProtagonistCharacter* aControlledCharacter;
		UCameraComponent*	 uPlayerCamera;
		// Camera based rotation
		float fMinimumVelocityForRotation = 2.f;
		UPROPERTY(EditAnywhere, meta=(DisplayName="Camera Based Character Rotation"))
		float fCameraBasedRotationSpeed = 300.f;
		// Camera sensitivity
		UPROPERTY(EditAnywhere, meta=(DisplayName="Horizontal Sensitivity"))
		float fHorizontalSensitivity = 50.f;
		UPROPERTY(EditAnywhere, meta=(DisplayName="Vertical Sensitivity"))
		float fVerticalSensitivity = 30.f;
		// Inventory
		UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> WidgetInventoryClass;
		UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
		UPlayerMenuWidget* WidgetInventory;
		bool bIsInventoryOpen = false;
	protected:
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaSeconds) override;
	public:
		// CONTROLS
		// Movement functions
		void MoveForward(float axisValue);
		void StrafeRight(float axisValue);
		// Camera
		void LookUp(float axisValue);
		void LookRight(float axisValue);
		// Camera based rotation
		void CameraRotationInterp(const float& DeltaSeconds, float& playerSpeed);
		virtual void SetupInputComponent() override;
		// Inventory HUD
		void ToggleInventory();
};
