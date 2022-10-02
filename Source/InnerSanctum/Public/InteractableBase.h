// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "InteractableBase.generated.h"

class UStaticMeshComponent;
class UDynamicMaterial;
UCLASS(Blueprintable)
class INNERSANCTUM_API AInteractableBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
	private:	
		
	protected:
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Params")
		TEnumAsByte<InteractionType> InteractionType = NoMovement;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* EmissiveMesh;
		UPROPERTY(EditAnywhere, Category = "Interaction Params")
		int EmissiveMaterialIndex = 0;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params")
		float EmissiveMaterialMaxStrength = 0.f;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params")
		float EmissiveMaterialDeltaTime = 0.f;
		UPROPERTY(EditAnywhere,BlueprintReadOnly, Category ="Interaction Params")
		FString PromptDescription;
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	public:	
		// Sets default values for this actor's properties
		AInteractableBase();
		// Called every frame
		virtual void Tick(float DeltaTime) override;
		//Getters
		UFUNCTION(BlueprintCallable)
		int GetEmissiveMaterialIndex() const { return EmissiveMaterialIndex; };

};
