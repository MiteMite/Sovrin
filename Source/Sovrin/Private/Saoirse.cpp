#include "Sovrin/Public/Saoirse.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MaterialHLSLTree.h"
#include "TimeTravelGlobal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Sovrin/Public/TimeTravel.h"
#include "Kismet/GameplayStatics.h"
#include "Sovrin/Public/SovrinHUD.h"
#include "Kismet/KismetStringLibrary.h"


ASaoirse::ASaoirse()
{
	//Load input mappings
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextObject(TEXT("/Game/SovrinClasses/InputMapping/IMC_Base.IMC_Base"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputForwardObject(TEXT("/Game/SovrinClasses/InputMapping/IA_MoveForward.IA_MoveForward"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRightObject(TEXT("/Game/SovrinClasses/InputMapping/IA_MoveRight.IA_MoveRight"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRewindObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Rewind.IA_Rewind"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputCrouchObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Crouch.IA_Crouch"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputFirstPersonToggleObject(TEXT("/Game/SovrinClasses/InputMapping/IA_FPSToggle.IA_FPSToggle"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputLookObject(TEXT("/Game/SovrinClasses/InputMapping/IA_FPSLook.IA_FPSLook"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputPauseMenuObject(TEXT("/Game/SovrinClasses/InputMapping/IA_PauseMenu.IA_PauseMenu"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> UAnimationClass(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny"));

	this->GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	InputMapping = InputMappingContextObject.Object;
	InputForwardAction = UInputForwardObject.Object;
	InputRightAction = UInputRightObject.Object;
	InputRewind = UInputRewindObject.Object;
	InputCrouch = UInputCrouchObject.Object;
	InputFirstPersonToggle = UInputFirstPersonToggleObject.Object;
	InputLook = UInputLookObject.Object;
	InputPauseMenu = UInputPauseMenuObject.Object;
	
	this->GetMesh()->SetSkeletalMeshAsset(USkeletalMeshObject.Object);
	this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	this->GetCharacterMovement()->bOrientRotationToMovement=false; //Character turns towards movement direction
	this->GetCharacterMovement()->RotationRate = FRotator(0.0f, 1.0f, 0.0f); // 1 rotation per second
	this->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	this->GetCharacterMovement()->GravityScale = 20.0f;
	this->GetCharacterMovement()->MaxWalkSpeedCrouched = 150.0f;

	//Create TimeTravel component (important for gameplay)
	TimeTravelComponent = CreateDefaultSubobject<UTimeTravel>(TEXT("TimeTravel"));

	// Create the CameraBoom (SpringArmComponent)
	SpringCam = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringCam->SetupAttachment(this->GetRootComponent());

	// Configure the CameraBoom properties
	SpringCam->TargetArmLength = 900.0f; // Set the distance between the camera and the pawn
	SpringCam->bUsePawnControlRotation = false; // Let the boom follow the pawn's control rotation
	SpringCam->bDoCollisionTest = false; // Don't want to collide with the pawn
	SpringCam->bEnableCameraLag = true; // Enable camera lag
	SpringCam->CameraLagSpeed = 10.0f; // Set the camera lag speed
	SpringCam->CameraLagMaxDistance = 1000.0f; // Set the maximum distance the camera can lag
	
	// Create the CameraComponent
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringCam); // Attach the camera to the CameraBoom
	
	// Configure the CameraComponent
	Camera->bUsePawnControlRotation = false; // Do not rotate the camera with the pawn
	SpringCam->SetRelativeRotation(DefaultRelativeRotation);
	SpringCam->SetRelativeLocation(DefaultRelativeLocation);
	
	//Create AI Stimuli Source
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	//bind animation blueprint
	if (UAnimationClass.Object!=nullptr)
	{
		this->GetMesh()->SetAnimInstanceClass(UAnimationClass.Object->GeneratedClass);
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Aint no animation blueprint"));
	}
}

void ASaoirse::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsFirstPersonMode)
	{
		UpdateRotationBasedOnMovement();
	}
}

void ASaoirse::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	TimeTravelGameMode = Cast<ATimeTravelGlobal>(UGameplayStatics::GetGameMode(World));
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping,0);
		}
	}
}

void ASaoirse::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(InputForwardAction,ETriggerEvent::Triggered, this, &ASaoirse::MoveForward);
		Input->BindAction(InputRightAction,ETriggerEvent::Triggered, this, &ASaoirse::MoveRight);
		Input->BindAction(InputRewind,ETriggerEvent::Started, this, &ASaoirse::RewindTime);
		Input->BindAction(InputRewind,ETriggerEvent::Completed, this, &ASaoirse::RewindTime);
		Input->BindAction(InputCrouch,ETriggerEvent::Started, this, &ASaoirse::CrouchProne);

		//camera toggle binding - triggers on press and release
		Input->BindAction(InputFirstPersonToggle, ETriggerEvent::Started, this, &ASaoirse::StartFirstPersonMode);
		Input->BindAction(InputFirstPersonToggle, ETriggerEvent::Completed, this, &ASaoirse::StopFirstPersonMode);

		//Mouse look binding
		Input->BindAction(InputLook, ETriggerEvent::Triggered, this, &ASaoirse::FirstPersonLook);

		Input->BindAction(InputPauseMenu, ETriggerEvent::Started, this, &ASaoirse::TogglePauseMenu);
	}
}

void ASaoirse::MoveForward(const FInputActionInstance& Inst)
{
	float InputValue = Inst.GetValue().Get<float>();

	//no movement in first person mode
	if (!bIsFirstPersonMode)
	{
		if (InputValue!=0.0f)
		{
			FVector MovementDirection;
			if (bIsFirstPersonMode)
			{
				if (Controller)
				{
					FRotator ControlRotation = Controller->GetControlRotation();
					MovementDirection = FRotationMatrix(FRotator(0.0f,ControlRotation.Yaw,0.0f)).GetUnitAxis(EAxis::X);
				}
			}
			else
			{
				if (Camera)
				{
					FVector CameraForward = Camera->GetForwardVector();
					//Project Camera forward onto the ground plane
					MovementDirection = FVector(CameraForward.X, CameraForward.Y, 0.0f).GetSafeNormal();

					if (MovementDirection.IsNearlyZero())
					{
						MovementDirection = this->GetActorForwardVector();
					}
				}
				else
				{
					MovementDirection = FVector::ForwardVector; // world fallback
				}
			}
		
			AddMovementInput(MovementDirection, InputValue);

		}
	}

}

void ASaoirse::MoveRight(const FInputActionInstance& Inst)
{
	float InputValue = Inst.GetValue().Get<float>();

	//no movement in first person mode
	if (!bIsFirstPersonMode)
	{
		if (InputValue!=0.0f)
		{
			FVector MovementDirection;
			if (bIsFirstPersonMode)
			{
				if (Controller)
				{
					FRotator ControlRotation = Controller->GetControlRotation();
					MovementDirection = FRotationMatrix(FRotator(0.0f,ControlRotation.Yaw,0.0f)).GetUnitAxis(EAxis::Y);
				}
			}
			else
			{
				if (Camera)
				{
					FVector CameraForward = Camera->GetRightVector();
					//Project Camera forward onto the ground plane
					MovementDirection = FVector(CameraForward.X, CameraForward.Y, 0.0f).GetSafeNormal();

					if (MovementDirection.IsNearlyZero())
					{
						MovementDirection = this->GetActorRightVector();
					}
				}
				else
				{
					MovementDirection = FVector::RightVector; // world fallback
				}
			}
		
			AddMovementInput(MovementDirection, InputValue);
		
		}
	}
	

}

void ASaoirse::CrouchProne(const FInputActionInstance& Inst)
{
	if (Inst.GetTriggerEvent()==ETriggerEvent::Started)
	{
		if (this->GetCharacterMovement()->bWantsToCrouch == false)
		{
			UE_LOG(LogTemp, Display, TEXT("Crouching %s"), this->GetCharacterMovement()->IsCrouching() ? TEXT("true") : TEXT("false"));
			this->GetCharacterMovement()->bWantsToCrouch = true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Crouching %s"), this->GetCharacterMovement()->IsCrouching() ? TEXT("true") : TEXT("false"));
			this->GetCharacterMovement()->bWantsToCrouch = false;
		}
	}
}

void ASaoirse::RewindTime(const FInputActionInstance& Inst)
{
	if (Inst.GetTriggerEvent()==ETriggerEvent::Started)
	{
		if (TimeTravelGameMode!=nullptr)
		{
			for (UTimeTravel* component : TimeTravelGameMode->TimeTravelActorsInWorld)
			{
				component->CallOnTimeTravelStarted();
			}
		}
	}
	if (Inst.GetTriggerEvent()==ETriggerEvent::Completed)
	{
		if (TimeTravelGameMode!=nullptr)
		{
			for (UTimeTravel* component : TimeTravelGameMode->TimeTravelActorsInWorld)
			{
				component->CallOnTimeTravelEnded();
			}
		}
	}
}

void ASaoirse::StartFirstPersonMode(const FInputActionInstance& Inst)
{
	UE_LOG(LogTemp, Display, TEXT("First person mode"));
	if (!bIsFirstPersonMode)
	{
		bIsFirstPersonMode = true;

		//disable rotation to movement for first person mode
		SpringCam->bInheritPitch = true;
		SpringCam->bInheritYaw = true;
		SpringCam->bInheritRoll = true;
		this->GetCharacterMovement()->bOrientRotationToMovement = false;
		this->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		
		FRotator InitialRotation = FRotator::ZeroRotator;
		if (Controller)
		{
			FVector CurrentVelocity = GetCharacterMovement()->Velocity;

			if (!CurrentVelocity.IsNearlyZero())
			{
				FVector MovementDirection = CurrentVelocity.GetSafeNormal();
				InitialRotation = MovementDirection.Rotation();

				InitialRotation.Pitch = 0.0f;
			}
			else
			{
				InitialRotation = GetActorRotation();
				InitialRotation.Pitch = 0.0f;
			}
			//set control rotation to face the movement direction
			Controller->SetControlRotation(InitialRotation);
		}
		
		//Switch to first person view
		SpringCam->TargetArmLength = 0.0f;
		SpringCam->SetRelativeLocation(FVector::ZeroVector); //head level
		SpringCam->SetRelativeRotation(FRotator::ZeroRotator); //face forward
		SpringCam->bDoCollisionTest = false;//disable collision
		SpringCam->bUsePawnControlRotation = true; //use pawn control rotation

		//hide character mesh in first person mode
		if (GetMesh())
		{
			GetMesh()->SetOwnerNoSee(true);
		}
	}
}

void ASaoirse::StopFirstPersonMode(const FInputActionInstance& Inst)
{
	UE_LOG(LogTemp, Display, TEXT("Stop first person mode"));
	if (bIsFirstPersonMode)
	{
		bIsFirstPersonMode = false;
		
		//re enable rotation to movement for first person mode
		SpringCam->bInheritPitch = false;
		SpringCam->bInheritYaw = false;
		SpringCam->bInheritRoll = false;
		this->GetCharacterMovement()->bOrientRotationToMovement = true;
		this->GetCharacterMovement()->bUseControllerDesiredRotation = false;

		//switch back to top down view
		SpringCam->TargetArmLength = DefaultTargetArmLength;
		SpringCam->SetRelativeLocation(DefaultRelativeLocation);
		SpringCam->SetRelativeRotation(DefaultRelativeRotation);
		SpringCam->bDoCollisionTest = true; //re enable collision
		SpringCam->bUsePawnControlRotation = false; //re enable pawn control rotation

		//reset camera rotation
		if (Controller)
		{
			FRotator CurrentRotation = Controller->GetControlRotation();
			Controller->SetControlRotation(FRotator(0.0,-180.0f,0.0f)); //reset camera rotation
		}
		
		//show character mesh in top down mode
		if (GetMesh())
		{
			GetMesh()->SetOwnerNoSee(false);
		}
	}
}

void ASaoirse::FirstPersonLook(const FInputActionInstance& Inst)
{
	if (!bIsFirstPersonMode)
	{
		return;
	}
	
	FVector2D LookAxisVector = Inst.GetValue().Get<FVector2D>();
	//Apply mouse sensitivity
	LookAxisVector *= MouseSensitivity;

	//Add yaw input
	AddControllerYawInput(LookAxisVector.X);

	//Add pitch input (negative for natural mouse feel)
	AddControllerPitchInput(-LookAxisVector.Y);
}

void ASaoirse::UpdateRotationBasedOnMovement()
{
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		FVector CurrentVelocity = GetCharacterMovement()->Velocity;
		
		if (CurrentVelocity.Size() > 0.1f)
		{
			
			FVector MovementDirection = FVector(CurrentVelocity.X, CurrentVelocity.Y, 0.0f).GetSafeNormal();
			FRotator TargetRotation = MovementDirection.Rotation();
			FRotator CurrentRotation = GetActorRotation();
			FRotator NewRotation = FRotator(CurrentRotation.Pitch, TargetRotation.Yaw, CurrentRotation.Roll);
			LastKnownCameraRotation = NewRotation;
			SetActorRotation(NewRotation);
		}
		else
		{
			SetActorRotation(LastKnownCameraRotation);
		}
	}
}

void ASaoirse::TogglePauseMenu(const FInputActionInstance& Inst)
{
	if (Inst.GetTriggerEvent()==ETriggerEvent::Started)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (ASovrinHUD* SovrinHUD = Cast<ASovrinHUD>(PlayerController->GetHUD()))
			{
				SovrinHUD->TogglePauseMenu();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No HUD found"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No player controller found"));
		}
	}
}

ASaoirse::~ASaoirse()
{
	
}

