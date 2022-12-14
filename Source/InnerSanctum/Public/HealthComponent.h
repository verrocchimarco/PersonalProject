// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABaseCharacter;
USTRUCT(BlueprintType)
struct FBodyPart
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere)
	float BodyPartMaxHealth;
	UPROPERTY(VisibleAnywhere)
	float BodyPartCurrentHealth;
	UPROPERTY(EditAnywhere)
	bool CanSurviveDismemberment;
	UPROPERTY(VisibleAnywhere)
	bool IsLimbAttached = true;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INNERSANCTUM_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathDelegate,ABaseCharacter*,deadCharacter);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDismembermentEvent,FName,brokenBoneName);
	private:
		UPROPERTY(EditAnywhere, Category="Health", meta=(DisplayName="Max Health"))
		float fMaxHealth = 100.f;
		UPROPERTY(VisibleAnywhere, Category="Health", meta=(DisplayName="Current Health"))
		float fCurrentHealth;

		UPROPERTY(EditAnywhere, Category="Dismemberment", meta=(DisplayName="Enable Dismemberment"))
		bool bIsDismembermentActive = true;
		UPROPERTY(EditAnywhere, Category="Dismemberment|Body Parts", meta=(DisplayName="Dismemberable Body Parts",EditCondition="bIsDismembermentActive"))
		TMap<FName, struct FBodyPart> mBodyParts;
		UPROPERTY(EditAnywhere, Category="Dismemberment|Body Parts", meta=(DisplayName="Bones matcher to body parts",EditCondition="bIsDismembermentActive"))
		TMap<FName, FName> mBonesMatching;
		UPROPERTY(EditAnywhere, Category="Dismemberment|Malus", meta=(DisplayName="Extra Damage upon Dismemberment",EditCondition="bIsDismembermentActive"))
		float fDismembermentExtraDamage = 15.f;
		UPROPERTY(EditAnywhere, Category="Dismemberment|Malus", meta=(DisplayName="Keeps losing Health after Dismemberment",EditCondition="bIsDismembermentActive"))
		bool bHasTickDamage = true;
		UPROPERTY(EditAnywhere, Category="Dismemberment|Malus", meta=(DisplayName="Looping damage delay",EditCondition="bHasTickDamage && bIsDismembermentActive"))
		float fDismembermentDamageLoopDelay = 3.5f;
		UPROPERTY(EditAnywhere, Category="Dismemberment|Malus", meta=(DisplayName="Looping damage amount",EditCondition="bHasTickDamage && bIsDismembermentActive"))
		float fDismembermentDamageLoopAmount = 5.f;
		FTimerHandle tDismembermentDamageTimer;

		ABaseCharacter* OwnerCharacter;
		// Sets default values for this component's properties
		UHealthComponent();

	protected:
		// Called when the game starts
		virtual void BeginPlay() override;

	public:
		UPROPERTY(BlueprintAssignable)
		FDeathDelegate OnDeathDelegate;
		UPROPERTY(BlueprintAssignable)
		FDismembermentEvent OnDismembermentEvent;
		// Called every frame
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		// Getters
		UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetMaxHealth() const { return fMaxHealth; }
		UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCurrentHealth() const { return fCurrentHealth; }
		UFUNCTION(BlueprintCallable)
		bool HealHealth(float healingAmount);
		UFUNCTION(BlueprintCallable)
		void ReduceHealth(float damageAmount);

		float EvaluateDismembermentDamage(float Damage,FName HitBoneName, const class UDamageType* DamageType);
		UFUNCTION()
		void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser );
		// DEBUG
		UFUNCTION(BlueprintCallable)
		void UpdateHealth(float healingAmount);
};
