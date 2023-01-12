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
		// Camera
			// Rotation
		UPROPERTY(EditAnywhere, Category="Camera Custom", meta=(DisplayName="Minimum Speed for Rotation"))
		float fMinimumVelocityForRotation = 2.f;
		UPROPERTY(EditAnywhere, Category="Camera Custom",meta=(DisplayName="Camera Based Character Rotation Speed"))
		float fCameraBasedRotationSpeed = 300.f;
		void CameraRotationInterp(const float& DeltaSeconds, float& playerSpeed);
			// Sensitivity
		UPROPERTY(EditAnywhere, Category="Camera Custom", meta=(DisplayName="Horizontal Sensitivity"))
		float fHorizontalSensitivity = 50.f;
		UPROPERTY(EditAnywhere, Category="Camera Custom", meta=(DisplayName="Vertical Sensitivity"))
		float fVerticalSensitivity = 30.f;
			// Aiming
		UPROPERTY(EditAnywhere, Category="Camera Custom", meta=(DisplayName="Aiming FOV"))
		float fAimFOV = 65.f;
		float fDefaultFOV;
		bool bIsAiming = false;
		FTimerHandle tFOVInterpTimer;
		float fAimInterpDelay = 0.1f;
		float fAimInterpProgress = 0.1f;
		// Inventory
		UPROPERTY(EditAnywhere, Category="Player Menu")
		TSubclassOf<UUserWidget> WidgetPlayerMenuClass;
		UPROPERTY(BlueprintReadWrite, Category="Player Menu", meta=(AllowPrivateAccess="true"))
		UPlayerMenuWidget* WidgetPlayerMenu;
		bool bIsInventoryOpen = false;
		// Movement
		bool bCanMove;
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
		void Aim();
		void StopAim();
		virtual void SetupInputComponent() override;
		// Inventory HUD
		UFUNCTION(BlueprintCallable)
		void ToggleInventory();
};
