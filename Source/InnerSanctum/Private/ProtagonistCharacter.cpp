// Fill out your copyright notice in the Description page of Project Settings.

#include "ProtagonistCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableBase.h"
#include "InteractInterface.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"


// Initialization and construction
AProtagonistCharacter::AProtagonistCharacter()
{
    PrimaryActorTick.bCanEverTick 	= true;
    
    uSpringArm           = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera SpringArm"));
    uCamera              = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    wInteractPrompt      = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Prompt"));
    uHealthComponent     = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
    uInventoryComponent  = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

    uSpringArm->SetupAttachment(RootComponent);
    uCamera->SetupAttachment(uSpringArm);
    wInteractPrompt->SetupAttachment(RootComponent);

    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->NavAgentProps.bCanSwim = false;
    GetCharacterMovement()->NavAgentProps.bCanJump = false;

    uSpringArm->bInheritPitch = true;
    uSpringArm->bInheritYaw = true;
    uSpringArm->bInheritRoll = false;
    uSpringArm->bUsePawnControlRotation = true;
    uCamera->bUsePawnControlRotation = true;
}
void AProtagonistCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    UE_LOG(LogTemp, Display, TEXT("Setup Player Input"));
    PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::StartSprint);
    PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AProtagonistCharacter::StopSprint);
    PlayerInputComponent->BindAction("Crouching", EInputEvent::IE_Released, this, &AProtagonistCharacter::ToggleCrouch);
    PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::CallInteraction);
    PlayerInputComponent->BindAction("VaultHide", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::CallMoveInteraction);
}

void AProtagonistCharacter::BeginPlay()
{
    Super::BeginPlay();
    // Set speed
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
    // Create interaction line traces
    GetWorldTimerManager().SetTimer(tInteractableLineCheckTimer,this,&AProtagonistCharacter::CastInteractableLineTrace,InteractableLineDelay,true);
    GetWorldTimerManager().SetTimer(tInteractableSphereCheckTimer,this,&AProtagonistCharacter::CastInteractableSphereTrace,InteractableSphereDelay,true);
    // Param to ignore self during LineTraceSingleByChannel cast
    CharacterIgnoreParam.AddIgnoredActor(GetUniqueID());
    wInteractPrompt->SetVisibility(false);
}
void AProtagonistCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Looping Events
void AProtagonistCharacter::CastInteractableLineTrace()
{
    AActor* HitActor;
    // Retrieve an object the character can interact with

    GetWorld()->LineTraceSingleByChannel(
                                        ActorInInteractionRange,
                                        uCamera->GetComponentLocation(),
										uCamera->GetComponentLocation() + uCamera->GetForwardVector()*InteractableFocusReach,
										ECollisionChannel::ECC_Visibility,
                                        CharacterIgnoreParam
    );
    // DrawDebugLine(GetWorld(), uCamera->GetComponentLocation(), uCamera->GetComponentLocation() + uCamera->GetForwardVector()*InteractableFocusReach, FColor::Orange,false,0.5f,0,4.f);
    HitActor = ActorInInteractionRange.GetActor();
    if(HitActor && HitActor->Implements<UInteractInterface>())
    {
        FocusedActor = HitActor;
        // UE_LOG(LogTemp, Display, TEXT("Actor in focus %s"), *(FocusedActor->GetName()));
    }
    else
        FocusedActor = nullptr;
    UpdateInteractionWidget(FocusedActor,ActorInInteractionRange.ImpactPoint);
    
}
void AProtagonistCharacter::CastInteractableSphereTrace()
{
    AActor* HitActor;
    TArray<FHitResult> HitObjectsInFocus;
    TArray<AActor*> placeholder_spheretrace_array;
    TArray<FHitResult> CurrentIterationActors;
    // Retrieve all interactable actors nearby
    UKismetSystemLibrary::SphereTraceMulti(
                                            GetWorld(),
                                            GetActorLocation()-GetActorForwardVector()*(InteractableFocusReach/2),
                                            GetActorLocation()+GetActorForwardVector()*(InteractableFocusReach/2),
                                            InteractableFocusReach*2,
                                            UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
                                            false,
                                            placeholder_spheretrace_array,
                                            EDrawDebugTrace::None,
                                            ActorsInFocus,
                                            true
                                            // FLinearColor::Blue,
                                            // FLinearColor::Green,
                                            // 1.f
    );
    CurrentIterationActors = ActorsInFocus;
    // Enable VisibilityGlow for all actors in sphere range if they implement the Interact Interface
    for(auto hitActor : CurrentIterationActors)
    {
        HitActor= hitActor.GetActor();
        if(HitActor && HitActor->Implements<UInteractInterface>())
        {
            if(!ActorsToIgnore.Contains(HitActor))
            {
                IInteractInterface::Execute_SetVisibilityGlow(HitActor);
                // Add newly set interactables actors to the ActorsToIgnore array so that they won't be retriggered in following calls
                ActorsToIgnore.Add(HitActor);
            }
        }
    }
    // Disable VisibilityGlow for all actors that are outside range
    for(auto alreadyFocused : ActorsToIgnore)
    {
        // Filter the CurrentIterationActors array with only the valid objects that implement the Interact Interface
        HitObjectsInFocus = CurrentIterationActors.FilterByPredicate([](const FHitResult& HitObject){ return HitObject.GetActor() && HitObject.GetActor()->Implements<UInteractInterface>();});
        // for(auto HitObject : HitObjectsInFocus)
        // {
        //    UE_LOG(LogTemp, Display, TEXT("HitActorInFocus that uses the interface [%s]"), *(HitObject.GetActor()->GetName())); 
        // }
        // If the actor in ActorsToIgnore is not in the list of valid actors that have been found by the sphere trace, remove it and unset the visibility glow
        if(!HitObjectsInFocus.ContainsByPredicate([&alreadyFocused](const FHitResult& HitObject){ return HitObject.GetActor()->GetName() == alreadyFocused->GetName();}))
        {
            // UE_LOG(LogTemp, Display, TEXT("Actor %s Not found"), *(alreadyFocused->GetName()));
            IInteractInterface::Execute_UnsetVisibilityGlow(alreadyFocused);
            ActorsToIgnore.Remove(alreadyFocused);
        }
    }
}
void AProtagonistCharacter::UpdateInteractionWidget_Implementation(AActor* InteractableActor,FVector TraceImpactPoint)
{
    if(InteractableActor)
    {
        wInteractPrompt->SetVisibility(true);
        wInteractPrompt->SetWorldLocation(FMath::VInterpTo(wInteractPrompt->GetComponentLocation(),TraceImpactPoint,GetWorld()->GetDeltaSeconds(),100.f));
    }
    else
        wInteractPrompt->SetVisibility(false);
}

// Movement Bindings

void AProtagonistCharacter::StartSprint()
{
    UE_LOG(LogTemp, Display, TEXT("Start Sprint"));
    if(GetCharacterMovement()->IsCrouching())
        UnCrouch();
    GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void AProtagonistCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AProtagonistCharacter::ToggleCrouch()
{
    UE_LOG(LogTemp, Display, TEXT("Toggling crouch"));
    if(GetCharacterMovement()->IsCrouching())
    {
        UE_LOG(LogTemp, Display, TEXT("Uncrouching"));
        UnCrouch();
    }
    else
    {
        StopSprint();
        UE_LOG(LogTemp, Display, TEXT("Crouching"));
        Crouch();
    }
}
// Interaction Bindings

void AProtagonistCharacter::CallInteraction()
{
    if(FocusedActor)
    {
        if(IInteractInterface::Execute_GetInteractionType(FocusedActor) == InteractionType::NoMovement)
            IInteractInterface::Execute_DoInteract(FocusedActor, this);
    }
}
void AProtagonistCharacter::CallMoveInteraction()
{
    if(FocusedActor)
    {
        if(IInteractInterface::Execute_GetInteractionType(FocusedActor) == InteractionType::MoveCharacter)
        {
            IInteractInterface::Execute_DoInteract(FocusedActor,this);
        }
    }
}
