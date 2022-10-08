// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class AProtagonistCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	private:
		UPROPERTY(EditAnywhere, Category="Health", meta=(DisplayName="Max Health"))
		float fMaxHealth = 100.f;
		UPROPERTY(EditAnywhere, Category="Health|Body Part Health", meta=(DisplayName="Head Health MaxHealth"))
		float fHeadMaxHealth = 100.f;
		UPROPERTY(EditAnywhere, Category="Health|Body Part Health", meta=(DisplayName="Upper Limbs MaxHealth"))
		float fUpperLimbsMaxHealth = 60.f;
		UPROPERTY(EditAnywhere, Category="Health|Body Part Health", meta=(DisplayName="Lower Limbs Health MaxHealth"))
		float fLowerLimbsMaxHealth = 60.f;
		UPROPERTY(VisibleAnywhere, Category="Health|Current Health", meta=(DisplayName="Current Health"))
		float fCurrentHealth;
		UPROPERTY(VisibleAnywhere, Category="Health|Current Health", meta=(DisplayName="Current Upper Limbs Health"))
		float fUpperLimbsCurrentHealth;
		UPROPERTY(VisibleAnywhere, Category="Health|Current Health", meta=(DisplayName="Current Lower Limbs Health"))
		float fLowerLimbsCurrentHealth;
		UPROPERTY(VisibleAnywhere, Category="Health|Current Health", meta=(DisplayName="Current Head Health"))
		float fHeadCurrentHealth;

		UPROPERTY(EditAnywhere, Category="Dismemberment Survival", meta=(DisplayName="Can Survive Beheading"))
		bool bNoHeadSurvive = false;
		UPROPERTY(EditAnywhere, Category="Dismemberment Survival", meta=(DisplayName="Can Survive Upper Limbs Dismemberment"))
		bool bNoUpLimbsSurvive = true;
		UPROPERTY(EditAnywhere, Category="Dismemberment Survival", meta=(DisplayName="Can Survive Lower Limbs Dismemberment"))
		bool bNoDownLimbsSurvive = false;

		AProtagonistCharacter* OwnerCharacter;
		// Sets default values for this component's properties
		UHealthComponent();

	protected:
		// Called when the game starts
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		// Getters
		UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetMaxHealth() const { return fMaxHealth; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCurrentHealth() const { return fCurrentHealth; }
		UFUNCTION()
		void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser );
};
