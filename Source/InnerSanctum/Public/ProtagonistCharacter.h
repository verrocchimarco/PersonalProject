// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProtagonistCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCapsuleComponent;
class UInputComponent;
class UWidgetComponent;
class UUserWidget;
class UHealthComponent;
class UInventoryComponent;
class USkeletalMeshComponent;
class USkeletalMesh;
UCLASS()
class INNERSANCTUM_API AProtagonistCharacter : public ACharacter
{
	GENERATED_BODY()
	public:	
		// Sets default values for this character's properties
		AProtagonistCharacter();
	private:
		// Actor Components
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="Camera Boom",AllowPrivateAccess="true"))
		USpringArmComponent* uSpringArm;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="Camera",AllowPrivateAccess="true"))
		UCameraComponent* 	 uCamera;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="Interact Prompt",AllowPrivateAccess="true"))
		UWidgetComponent* wInteractPrompt;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",meta=(DisplayName="Health Component",AllowPrivateAccess="true"))
		UHealthComponent* uHealthComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",meta=(DisplayName="Inventory Component",AllowPrivateAccess="true"))
		UInventoryComponent* uInventoryComponent;
		// Movement speed params
		UPROPERTY(EditAnywhere, Category = "Movement", meta=(DisplayName="Walking Speed"))
		float MaxWalkSpeed = 220.f;
		UPROPERTY(EditAnywhere, Category = "Movement", meta=(DisplayName="Sprinting Speed"))
		float MaxRunSpeed = 400.f;
		UPROPERTY(EditAnywhere, Category = "Movement", meta=(DisplayName="Crouch Speed"))
		float MaxCrouchSpeed = 100.f;
		// Interaction with InteractInterface actors
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableFocusReach = 300.f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableActiveReach = 100.f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableSphereDelay = .5f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableLineDelay = .09f;

		FText InteractableDescription;
		FTimerHandle tInteractableSphereCheckTimer,tInteractableLineCheckTimer;
		FHitResult ActorInInteractionRange;
		UPROPERTY()
		TArray<FHitResult> ActorsInFocus;
		UPROPERTY()
		AActor* FocusedActor;
		TArray<AActor *, FDefaultAllocator> ActorsToIgnore;
		FCollisionQueryParams CharacterIgnoreParam;	
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
		UFUNCTION()
		virtual void CastInteractableLineTrace();
		UFUNCTION()
		virtual void CastInteractableSphereTrace();

	public:
		// Called every frame
		virtual void Tick(float DeltaTime) override;
		// Getters
		UFUNCTION(BlueprintCallable)
		UCameraComponent* GetCamera() const { return uCamera; };
		UFUNCTION(BlueprintCallable)
		USpringArmComponent* GetSpringArm() const { return uSpringArm; };
		UFUNCTION(BlueprintCallable)
		UWidgetComponent* GetInteractPrompt() const { return wInteractPrompt; };
		
		// Movement
		void StartSprint();
		void StopSprint();
		void ToggleCrouch();

		// Interaction
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateInteractionWidget(AActor* InteractableActor, FVector TraceImpactPoint);
		void CallInteraction();
		void CallMoveInteraction();
};
