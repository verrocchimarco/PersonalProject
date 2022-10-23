// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;
UCLASS(Abstract)
class INNERSANCTUM_API ABaseCharacter : public ACharacter
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

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	public:
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		UFUNCTION(BlueprintCallable, BlueprintPure)
		UHealthComponent* GetHealthComponent() const { return uHealthComponent; };
		// Movement
		virtual void StartSprint();
		virtual void StopSprint();
		virtual void ToggleCrouch();
};
