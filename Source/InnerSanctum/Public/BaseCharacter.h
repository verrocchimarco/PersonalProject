// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;

USTRUCT(BlueprintType)
struct FDismemberedMesh
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> StumpMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> LostLimbMesh;
};

UCLASS(Abstract)
class INNERSANCTUM_API ABaseCharacter : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	public:	
		// Sets default values for this character's properties
		ABaseCharacter();
	protected:
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",meta=(DisplayName="Health Component",AllowPrivateAccess="true"))
		UHealthComponent* uHealthComponent;
		// Movement speed params
		UPROPERTY(EditAnywhere, Category = "Movement", meta=(DisplayName="Walking Speed"))
		float MaxWalkSpeed = 220.f;
		UPROPERTY(EditAnywhere, Category = "Movement", meta=(DisplayName="Sprinting Speed"))
		float MaxRunSpeed = 400.f;
		UPROPERTY(EditAnywhere, Category = "Movement", meta=(DisplayName="Crouch Speed"))
		float MaxCrouchSpeed = 100.f;
		UPROPERTY(VisibleAnywhere, Category = "Movement", meta=(DisplayName="Running State"))
		bool bIsRunning = false;
		// Resting params
		UPROPERTY(VisibleAnywhere)
		bool bIsResting = false;
		UPROPERTY(EditAnywhere)
		float fHealthRecoveryDelay = 0.8f;
		UPROPERTY(EditAnywhere)
		float fHealthRecoveryWhileResting = 15.f;
		FTimerHandle tHealthRecoveryTimer;
		// Dismemberment
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dismemberment", meta=(DisplayName="Dismemberable Body Parts Meshes"))
		TMap<FName, struct FDismemberedMesh> mBodyParts;
	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
		FGameplayTagContainer TagSpawnManager;
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
		UFUNCTION(BlueprintImplementableEvent)
		void SpawnDismemberedMesh(FName brokenBoneName);
	public:
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		UFUNCTION(BlueprintCallable, BlueprintPure)
		UHealthComponent* GetHealthComponent() const { return uHealthComponent; };
		// Movement
		virtual void StartSprint();
		virtual void StopSprint();
		virtual void ToggleCrouch();
		UFUNCTION(BlueprintPure)
		virtual bool GetIsRunning() const { return bIsRunning; }
		// Resting
		void BeginResting();
		void HealWhileResting();
		void StopResting();
		UFUNCTION(BlueprintPure)
		bool GetIsResting() const { return bIsResting;}
		virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = TagSpawnManager; return; }
};
