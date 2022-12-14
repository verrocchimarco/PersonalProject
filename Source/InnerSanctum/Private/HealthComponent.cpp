// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "ProtagonistCharacter.h"
#include "InnerSanctumGameModeBase.h"
#include "Kismet/GameplayStatics.h"
// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter 				= Cast<ABaseCharacter>(GetOwner());
	fCurrentHealth 				= fMaxHealth;
	if(bIsDismembermentActive)
	{
		// Set current health for body parts
		for(TTuple<FName,FBodyPart>& bodyPart : mBodyParts)
		{
			bodyPart.Value.BodyPartCurrentHealth = bodyPart.Value.BodyPartMaxHealth;
		}
		// Sanity check on bones mapping
		for(auto bone : mBonesMatching)
		{
			if(!mBodyParts.Contains(bone.Value))
			{
				UE_LOG(LogTemp, Warning, TEXT("Bone %s is not set in the dismemberable body parts map"), *(bone.Value.ToString()));
			}
		}
	}
	OwnerCharacter->OnTakePointDamage.AddDynamic(this, &UHealthComponent::OnTakePointDamage);
}





// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

/*
Evaluate total damage after checking if any limb has been dismembered.
Evaluation is started if parameter bIsDismembermentActive is true and the DamageType is recognized as the component's weak point.
If one of the limbs' health drops to zero, broadcast dismemberment event
*/ 
float UHealthComponent::EvaluateDismembermentDamage(float Damage,FName HitBoneName, const class UDamageType* DamageType)
{
	// Check if the hit bone is part of the dismemberment map
	if(!mBonesMatching.Contains(HitBoneName))
	{
		return Damage;
	}
	FName targetedBodyPart = mBonesMatching[HitBoneName];
	mBodyParts[targetedBodyPart].BodyPartCurrentHealth = FMath::Max(mBodyParts[targetedBodyPart].BodyPartCurrentHealth - Damage, 0.f);
	UE_LOG(LogTemp, Display, TEXT("Health Component: Applying damage to bone %s. Current body part health: %f"),*(targetedBodyPart.ToString()),mBodyParts[targetedBodyPart].BodyPartCurrentHealth);
	// Compute final damage output if the body part has been dismembered
	if(!mBodyParts[targetedBodyPart].BodyPartCurrentHealth && mBodyParts[targetedBodyPart].IsLimbAttached)
	{
		// Set body part as detached
		mBodyParts[targetedBodyPart].IsLimbAttached = false;
		// Broadcast loss of limb
		if(OnDismembermentEvent.IsBound())
		{
			OnDismembermentEvent.Broadcast(targetedBodyPart);
		}
		// Kill character if the dismemberment event is fatal
		if(mBodyParts[targetedBodyPart].CanSurviveDismemberment)
		{
			// Check if character should keep taking damage upon dismemberment. Don't enable it if the timer has been enabled already
			if(bHasTickDamage && !GetWorld()->GetTimerManager().IsTimerActive(tDismembermentDamageTimer))
			{
				FTimerDelegate DismembermentLoopDamageDelegate = FTimerDelegate::CreateUObject(this, &UHealthComponent::ReduceHealth, fDismembermentDamageLoopAmount);
				GetWorld()->GetTimerManager().SetTimer(tDismembermentDamageTimer,DismembermentLoopDamageDelegate,-1.f,true,fDismembermentDamageLoopDelay*1.7f);
			}
			UE_LOG(LogTemp, Display, TEXT("Health Component: Bone %s got dismembered. Applying bonus damage"),*(targetedBodyPart.ToString()));
			return Damage + fDismembermentExtraDamage;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Health Component: Bone %s got dismembered. Survival for this bone is set to false"),*(targetedBodyPart.ToString()));
			return fMaxHealth;
		}

	}
	else
	{
		return Damage;
	}
}
void UHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser )
{
	UE_LOG(LogTemp, Display, TEXT("Health Component: Damage taken. HitLocation: %s | Bone Hit: %s"), *(HitLocation.ToCompactString()), *(BoneName.ToString()));
	if(bIsDismembermentActive)
	{
		ReduceHealth(EvaluateDismembermentDamage(Damage,BoneName,DamageType));
	}
	else
	{
		ReduceHealth(Damage);
	}
}

bool UHealthComponent::HealHealth(float healingAmount)
{
	if(fCurrentHealth < fMaxHealth)
	{
		fCurrentHealth = FMath::Min(fCurrentHealth+healingAmount, fMaxHealth);
		UE_LOG(LogTemp, Display, TEXT("Health Component: Healing. New Health: [%d]"),fCurrentHealth);
		// Heal limbs if dismemberment is active
		if(bIsDismembermentActive)
		{
			for(TTuple<FName,FBodyPart>& bodyPart : mBodyParts)
			{
				bodyPart.Value.BodyPartCurrentHealth = FMath::Min(bodyPart.Value.BodyPartCurrentHealth+healingAmount, bodyPart.Value.BodyPartMaxHealth);
				UE_LOG(LogTemp, Display, TEXT("Health Component: Healing limb %s. New Health: [%d]"),*(bodyPart.Key.ToString()),bodyPart.Value.BodyPartCurrentHealth);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}
void UHealthComponent::ReduceHealth(float damageAmount)
{
	fCurrentHealth = FMath::Max( fCurrentHealth-damageAmount, 0.f);
	UE_LOG(LogTemp, Display, TEXT("Health Component: Damage taken. Current Health: %f"), fCurrentHealth);
	if(!fCurrentHealth && OnDeathDelegate.IsBound())
	{
		GetWorld()->GetTimerManager().ClearTimer(tDismembermentDamageTimer);
		OnDeathDelegate.Broadcast(OwnerCharacter);
	}
}
void UHealthComponent::UpdateHealth(float healingAmount)
{
	fCurrentHealth = FMath::Clamp(fCurrentHealth+healingAmount,0.f, fMaxHealth);
}