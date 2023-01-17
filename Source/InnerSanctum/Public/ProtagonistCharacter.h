// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InteractInterface.h"
#include "ProtagonistCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCapsuleComponent;
class UInputComponent;
class UInteractionPromptWidget;
class UHealthComponent;
class UInventoryComponent;
class USkeletalMeshComponent;
class USkeletalMesh;
class UInteractionPromptWidget;

enum QuickItem
{
	QuickItem1 = 1,
	QuickItem2,
	QuickItem3,
	QuickItem4
};
UCLASS()
class INNERSANCTUM_API AProtagonistCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	DECLARE_DELEGATE_OneParam(FInteractionInputDelegate, InteractionType);
	DECLARE_DELEGATE_OneParam(FQuickItemSelect, QuickItem);
	public:	
		// Sets default values for this character's properties
		AProtagonistCharacter();
	protected:
		// Actor Components
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="Camera Boom",AllowPrivateAccess="true"))
		USpringArmComponent* uSpringArm;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="Camera",AllowPrivateAccess="true"))
		UCameraComponent* 	 uCamera;
		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta=(DisplayName="Interact Prompt",AllowPrivateAccess="true"))
		UWidgetComponent* wInteractPrompt;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",meta=(DisplayName="Inventory Component",AllowPrivateAccess="true"))
		UInventoryComponent* uInventoryComponent;
		UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* uBackpackMesh;
		// Interaction with InteractInterface actors
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableFocusReach = 300.f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableActiveReach = 100.f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableSphereDelay = .5f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableLineDelay = .09f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableLineCapsuleRadius = 5.5f;
		UPROPERTY(EditAnywhere, Category = "Interactions")
		float InteractableLineCapsuleHalfHeight = 1.f;
		FText InteractableDescription;
		FTimerHandle tInteractableSphereCheckTimer,tInteractableLineCheckTimer; 
		UPROPERTY()
		AActor* FocusedActor;
		FCollisionQueryParams CharacterIgnoreParam;	
		// Equipped Item usage
		bool bIsReadyToUseEquip = false;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Usable Items", meta=(AllowPrivateAccess="true",DisplayName="Item attachment socket"))
		FString sRightArmSocket;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Usable Items", meta=(AllowPrivateAccess="true",DisplayName="Flashlight attachment socket"))
		FString sLeftArmSocket;

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
		UFUNCTION(BlueprintCallable, BlueprintPure)
		UCameraComponent* GetCamera() const { return uCamera; };
		UFUNCTION(BlueprintCallable, BlueprintPure)
		USpringArmComponent* GetSpringArm() const { return uSpringArm; };
		UFUNCTION(BlueprintCallable, BlueprintPure)
		UWidgetComponent* GetInteractPrompt() const { return wInteractPrompt; };
		UFUNCTION(BlueprintCallable, BlueprintPure)
		UInventoryComponent* GetInventoryComponent() const { return uInventoryComponent; };
		UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetItemBoneSocket() const { return sRightArmSocket; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetSecondaryItemBoneSocket() const { return sLeftArmSocket; }
		UStaticMeshComponent* GetBackpackMesh() const { return uBackpackMesh; }
		bool IsDead() const;
		// Movement
		virtual void StartSprint() override;
		
		// Interaction
		void UpdateInteractionWidget(AActor* InteractableActor, FVector TraceImpactPoint);
		void CallInteraction(InteractionType ButtonPressed);
		
		// Equipped Item usage
		void SetIsEquipReady(bool newReady);
		void ToggleDrawEquippedItem();
		void UseEquippedItem();
		void EquipQuickItem(QuickItem selectedItem);
};
