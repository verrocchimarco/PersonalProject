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
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "InteractionPromptWidget.h"


// Initialization and construction
AProtagonistCharacter::AProtagonistCharacter()
{    
    uSpringArm           = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera SpringArm"));
    uCamera              = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    wInteractPrompt      = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interact Prompt"));
    uInventoryComponent  = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
    uBackpackMesh        = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backpack Mesh"));

    uSpringArm->SetupAttachment(GetMesh(),"clavicle_CameraSocket");
    uCamera->SetupAttachment(uSpringArm);
    wInteractPrompt->SetupAttachment(RootComponent);
    uBackpackMesh->SetupAttachment(GetMesh(),"spine_BackpackSocket");
    uBackpackMesh->SetVisibility(false);
    
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->NavAgentProps.bCanSwim = false;
    GetCharacterMovement()->NavAgentProps.bCanJump = false;
    
    // Prevent camera from rotating the character
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Make the spring arm follow the character rotation
    uSpringArm->bInheritPitch = true;
    uSpringArm->bInheritYaw = true;
    uSpringArm->bInheritRoll = false;
    uSpringArm->bUsePawnControlRotation = true;
    uCamera->bUsePawnControlRotation = true;
    
    uSpringArm->bEnableCameraLag = true;

    sRightArmSocket = nullptr;
    sLeftArmSocket = nullptr;
}
void AProtagonistCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction<FInteractionInputDelegate>("InteractE", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::CallInteraction, InteractionType::EButton);
    PlayerInputComponent->BindAction<FInteractionInputDelegate>("InteractSpace", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::CallInteraction, InteractionType::SpaceButton);
    PlayerInputComponent->BindAction<FQuickItemSelect>("QuickItem1", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::EquipQuickItem, QuickItem1);
    PlayerInputComponent->BindAction<FQuickItemSelect>("QuickItem2", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::EquipQuickItem, QuickItem2);
    PlayerInputComponent->BindAction<FQuickItemSelect>("QuickItem3", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::EquipQuickItem, QuickItem3);
    PlayerInputComponent->BindAction<FQuickItemSelect>("QuickItem4", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::EquipQuickItem, QuickItem4);
    PlayerInputComponent->BindAction("ToggleDraw", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::ToggleDrawEquippedItem);
    PlayerInputComponent->BindAction("UseItem", EInputEvent::IE_Pressed, this, &AProtagonistCharacter::UseEquippedItem);
}

void AProtagonistCharacter::BeginPlay()
{
    Super::BeginPlay();
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
    TArray<AActor*> placeholder_array;
    FHitResult ActorInInteractionRange;
    // Retrieve an object the character can interact with

    UKismetSystemLibrary::CapsuleTraceSingle(
        GetWorld(),
        uCamera->GetComponentLocation(),
        uCamera->GetComponentLocation() + uCamera->GetForwardVector()*InteractableFocusReach,
        InteractableLineCapsuleRadius,
        InteractableLineCapsuleHalfHeight,
        UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
        false,
        placeholder_array,
        EDrawDebugTrace::None,
        ActorInInteractionRange,
        true
    );
    // DrawDebugLine(GetWorld(), uCamera->GetComponentLocation(), uCamera->GetComponentLocation() + uCamera->GetForwardVector()*InteractableFocusReach, FColor::Orange,false,0.5f,0,4.f);
    HitActor = ActorInInteractionRange.GetActor();
    if(HitActor && HitActor->Implements<UInteractInterface>())
    {
        FocusedActor = HitActor;    
    }
    else
    {
        FocusedActor = nullptr;
    }
    UpdateInteractionWidget(FocusedActor,ActorInInteractionRange.Location);
}
void AProtagonistCharacter::CastInteractableSphereTrace()
{
    AActor* HitActor;
    TArray<AActor*> placeholder_spheretrace_array;
    TArray<FHitResult> ActorsInFocus;
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
                                            true,
                                            FLinearColor::Yellow
    );
    // Enable VisibilityGlow for all actors in sphere range if they implement the Interact Interface
    for(auto hitObject : ActorsInFocus)
    {
        HitActor = hitObject.GetActor();
        if(HitActor && HitActor->Implements<UInteractInterface>())
        {
            IInteractInterface::Execute_SetVisibilityGlow(HitActor);
        }
    }
}
void AProtagonistCharacter::UpdateInteractionWidget(AActor* InteractableActor,FVector TraceImpactPoint)
{
    if(InteractableActor)
    {
        FText temp;
        wInteractPrompt->SetVisibility(true);
        wInteractPrompt->SetWorldLocation(FMath::VInterpTo(wInteractPrompt->GetComponentLocation(),TraceImpactPoint,GetWorld()->GetDeltaSeconds(),250.f));
        if(!Cast<UInteractionPromptWidget>(wInteractPrompt->GetUserWidgetObject()))
        {
            UE_LOG(LogTemp, Display, TEXT("Protagonist Character: UserWidget is NULL"));    
            return;
        }
        temp = IInteractInterface::Execute_GetPromptDescriptionE(InteractableActor);
        Cast<UInteractionPromptWidget>(wInteractPrompt->GetUserWidgetObject())->SetPromptDescriptionE(temp);
        temp = IInteractInterface::Execute_GetPromptDescriptionSpace(InteractableActor);
        Cast<UInteractionPromptWidget>(wInteractPrompt->GetUserWidgetObject())->SetPromptDescriptionSpace(temp);
    }
    else
        wInteractPrompt->SetVisibility(false);
}

// Movement Bindings

void AProtagonistCharacter::StartSprint()
{
    if(!bIsReadyToUseEquip)
    {
        Super::StartSprint();
    }
}
// Interaction Bindings

void AProtagonistCharacter::CallInteraction(InteractionType ButtonPressed)
{
    if(FocusedActor)
    {
        InteractionType FocusedActorInteractType = IInteractInterface::Execute_GetInteractionType(FocusedActor);
        if(ButtonPressed == InteractionType::EButton)
        {
            UE_LOG(LogTemp, Display, TEXT("Interacting with key: E"));
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("Interacting with key: space"));
        }
        if( FocusedActorInteractType == ButtonPressed || FocusedActorInteractType == InteractionType::DualInteraction )
            IInteractInterface::Execute_DoInteract(FocusedActor, this, ButtonPressed);
    }
}

// Equipment
void AProtagonistCharacter::SetIsEquipReady(bool newReady)
{
    bIsReadyToUseEquip = newReady;
    if(bIsReadyToUseEquip)
    {
        UE_LOG(LogTemp, Display, TEXT("Aiming with equipped item"));
        StopSprint();
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Not aiming with equipped item"));
    }
}

void AProtagonistCharacter::ToggleDrawEquippedItem()
{
    uInventoryComponent->ToggleDrawEquippedItem();
}

// Use equipped item if the item is drawn and ready to be used. If the item usage has expired, bIsEquipDrawn will be set to false
void AProtagonistCharacter::UseEquippedItem()
{
    if(bIsReadyToUseEquip && uInventoryComponent->IsEquippedItemDrawn())
    {
        uInventoryComponent->UseEquippedItem();
    }
    UE_LOG(LogTemp, Display, TEXT("Is Equip still drawn: %d"),uInventoryComponent->IsEquippedItemDrawn());
}

bool AProtagonistCharacter::IsDead() const
{
    return uHealthComponent->IsDead();
}

void AProtagonistCharacter::EquipQuickItem(QuickItem selectedItem)
{
    UE_LOG(LogTemp, Display, TEXT("You're trying to equip the quick item"));
    uInventoryComponent->EquipQuickItem(selectedItem);
}