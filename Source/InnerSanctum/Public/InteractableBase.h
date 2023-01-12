// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "InteractableBase.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
UCLASS(Blueprintable)
class INNERSANCTUM_API AInteractableBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* EmissiveMesh;
		UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		UMaterialInstanceDynamic* DynamicEmissiveMaterial;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Params|Emissive Material")
		int EmissiveMaterialIndex = 0;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Emissive Material")
		float EmissiveMaterialMaxStrength = .4f;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Params|Emissive Material")
		float EmissiveMaterialDeltaTime = .2f;
		
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction Params|Player Interaction", meta=(DisplayName="Interaction Input Type"))
		TEnumAsByte<InteractionType> InteractableButtonResponse = EButton;
		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category ="Interaction Params|Player Interaction", meta=(DisplayName="Prompt Description - E", EditCondition="InteractableButtonResponse==InteractionType::EButton || InteractableButtonResponse==InteractionType::DualInteraction"))
		FText PromptDescriptionE;
		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category ="Interaction Params|Player Interaction", meta=(DisplayName="Prompt Description - Space", EditCondition="InteractableButtonResponse==InteractionType::SpaceButton || InteractableButtonResponse==InteractionType::DualInteraction"))
		FText PromptDescriptionSpace;
		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category ="Interaction Params|Player Interaction", meta=(DisplayName="Interaction Enabled"))
		bool bCanBeInteracted = true;
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	public:	
		// Sets default values for this actor's properties
		AInteractableBase();
		// Called every frame
		virtual void Tick(float DeltaTime) override;
		//Getters
		UFUNCTION()
		int GetEmissiveMaterialIndex() const { return EmissiveMaterialIndex; };
		UFUNCTION()
		UMaterialInstanceDynamic* GetEmissiveMaterial() const { return DynamicEmissiveMaterial; }
		void SetEmissiveMesh(UStaticMesh* newMesh);
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void InteractionE(AActor* InteractingActor);
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void InteractionSpace(AActor* InteractingActor);
};
