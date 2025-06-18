#include "Sovrin/Public/Saoirse.h"

#include "EditorCategoryUtils.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MaterialHLSLTree.h"
#include "TimeTravelGlobal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameMode.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	CameraAdjustToGroundLevel(DeltaSeconds);
	
	UpdateRotationBasedOnMovement();
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
	Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (Input)
	{
		Input->BindAction(InputForwardAction,ETriggerEvent::Triggered, this, &ASaoirse::MoveForward);
		Input->BindAction(InputForwardAction,ETriggerEvent::Completed, this, &ASaoirse::MoveForwardCompleted);
		Input->BindAction(InputRightAction,ETriggerEvent::Triggered, this, &ASaoirse::MoveRight);
		Input->BindAction(InputRightAction,ETriggerEvent::Completed, this, &ASaoirse::MoveRightCompleted);
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
	CurrentForwardInput = InputValue;
	FVector TraceStartingPoint = GetActorLocation();
	FVector TraceEndPoint = FVector::ZeroVector; //short range
	//no movement in first person mode
	if (!bIsFirstPersonMode && InputValue!=0.0f)
	{
		FVector MovementDirection;

		//Calculate movement direction based on camera orientation
		if (Camera)
		{
			// Get camera's forward vector and project it onto the horizontal plane
			FVector CameraForward = Camera->GetForwardVector();
			MovementDirection = FVector(CameraForward.X, CameraForward.Y, 0.0f).GetSafeNormal();
		}
		else
		{
			MovementDirection = FVector::ForwardVector; // world fallback
		}

		// If in cover state, restrict movement to parallel to wall only
		if (bIsInCoverState && !CoverWallNormal.IsZero())
		{
			// Check if input is pointing away from wall to exit cover
			float DotWithOppositeNormal = FVector::DotProduct(MovementDirection * InputValue, CoverWallNormal);
			
			if (DotWithOppositeNormal > 0.5f) // Input pointing away from wall with sufficient strength
			{
				// Exit cover and allow normal movement
				CoverWallNormal = FVector::ZeroVector;
				ExitCoverState();
				AddMovementInput(MovementDirection, InputValue);
				return;
			}
			
			// Restrict movement to sideways only (parallel to wall)
			FVector SidewaysDirection = FVector::CrossProduct(CoverWallNormal, FVector::UpVector).GetSafeNormal();
			float SidewaysInput = FVector::DotProduct(MovementDirection, SidewaysDirection) * InputValue;
			
			// Only allow sideways movement - block forward/backward relative to wall
			if (SidewaysInput != 0.0f)
			{
				FVector IntendedDirection = SidewaysDirection * FMath::Sign(SidewaysInput);
				FVector EdgeLocation;
				
				if (IsWallEdgeDetected(IntendedDirection, EdgeLocation))
				{
					// Calculate direction from character to edge
					FVector CharacterToEdge = (EdgeLocation - GetActorLocation()).GetSafeNormal();
					
					// Project this onto the sideways direction to see which side the edge is on
					float EdgeSideAlignment = FVector::DotProduct(CharacterToEdge, SidewaysDirection);
					float MovementSideAlignment = FVector::DotProduct(IntendedDirection, SidewaysDirection);
					
					// If the edge is on the same side as the intended movement, block it
					if (FMath::Sign(EdgeSideAlignment) == FMath::Sign(MovementSideAlignment))
					{
						UE_LOG(LogTemp, Warning, TEXT("Wall edge detected - blocking movement toward edge"));
						return;
					}
					else
					{
						// Edge is on the opposite side, allow movement
						AddMovementInput(SidewaysDirection, SidewaysInput);
					}
				}
				else
				{
					// No edge detected, allow normal sideways movement
					AddMovementInput(SidewaysDirection, SidewaysInput);
				}
			}
			// If no sideways input, don't move at all
			return;
		}

		// Normal movement logic continues for non-cover state...
		FVector CameraRight = Camera ? Camera->GetRightVector() : FVector::RightVector;
		FVector CameraRightGrounded = FVector(CameraRight.X, CameraRight.Y, 0.0f).GetSafeNormal();
		
		FVector ActualMovementDirection = (MovementDirection * CurrentForwardInput) + (CameraRightGrounded * CurrentRightInput);
		ActualMovementDirection = ActualMovementDirection.GetSafeNormal();

		TraceEndPoint = TraceStartingPoint + (ActualMovementDirection * 100.0f);

		FHitResult HitResult;
		FCollisionQueryParams LineTraceParameters;
		LineTraceParameters.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartingPoint, TraceEndPoint, ECC_Visibility, LineTraceParameters);

		FColor LineColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugLine(GetWorld(), TraceStartingPoint, TraceEndPoint, LineColor, false, 1.0f, 0, 2.0f);
		
		if (bHit)
		{
			float DotProduct = FVector::DotProduct(HitResult.Normal, ActualMovementDirection);
			
			if (DotProduct < 0.0f)
			{
				if (!bIsInCoverState) 
				{
					CoverWallNormal = HitResult.Normal;
					EnterCoverState();
				}
				FVector SlideDirection = FVector::VectorPlaneProject(ActualMovementDirection, HitResult.Normal);
				AddMovementInput(SlideDirection, InputValue);
				return;
			}
		}
		else
		{
			if (bIsInCoverState)
			{
				CoverWallNormal = FVector::ZeroVector;
				ExitCoverState();
			}
		}
		AddMovementInput(MovementDirection, InputValue);
	}
	else
	{
		CurrentForwardInput = 0.0f;
	}
}

void ASaoirse::MoveRight(const FInputActionInstance& Inst)
{
	float InputValue = Inst.GetValue().Get<float>();
	CurrentRightInput = InputValue;
	if (!bIsFirstPersonMode && InputValue!=0.0f)
	{
		FVector MovementDirection;
		
		if (Camera)
		{
			FVector CameraRight = Camera->GetRightVector();
			MovementDirection = FVector(CameraRight.X, CameraRight.Y, 0.0f).GetSafeNormal();
		}
		else
		{
			MovementDirection = FVector::RightVector;
		}

		// If in cover state, restrict movement to parallel to wall only
		if (bIsInCoverState && !CoverWallNormal.IsZero())
		{
			// Check if input is pointing away from wall to exit cover
			float DotWithOppositeNormal = FVector::DotProduct(MovementDirection * InputValue, CoverWallNormal);
			
			if (DotWithOppositeNormal > 0.5f)
			{
				CoverWallNormal = FVector::ZeroVector;
				ExitCoverState();
				AddMovementInput(MovementDirection, InputValue);
				return;
			}
			
			// Restrict movement to sideways only (parallel to wall)
			FVector SidewaysDirection = FVector::CrossProduct(CoverWallNormal, FVector::UpVector).GetSafeNormal();
			float SidewaysInput = FVector::DotProduct(MovementDirection, SidewaysDirection) * InputValue;
			
			// Only allow sideways movement - block forward/backward relative to wall
			if (SidewaysInput != 0.0f)
			{
				FVector IntendedDirection = SidewaysDirection * FMath::Sign(SidewaysInput);
				FVector EdgeLocation;
				
				if (IsWallEdgeDetected(IntendedDirection, EdgeLocation))
				{
					// Calculate direction from character to edge
					FVector CharacterToEdge = (EdgeLocation - GetActorLocation()).GetSafeNormal();
					
					// Project this onto the sideways direction to see which side the edge is on
					float EdgeSideAlignment = FVector::DotProduct(CharacterToEdge, SidewaysDirection);
					float MovementSideAlignment = FVector::DotProduct(IntendedDirection, SidewaysDirection);
					
					// If the edge is on the same side as the intended movement, block it
					if (FMath::Sign(EdgeSideAlignment) == FMath::Sign(MovementSideAlignment))
					{
						UE_LOG(LogTemp, Warning, TEXT("Wall edge detected - blocking movement toward edge"));
						return;
					}
					else
					{
						// Edge is on the opposite side, allow movement
						AddMovementInput(SidewaysDirection, SidewaysInput);
					}
				}
				else
				{
					// No edge detected, allow normal sideways movement
					AddMovementInput(SidewaysDirection, SidewaysInput);
				}
			}
			// If no sideways input, don't move at all
			return;
		}

		// Normal movement logic continues for non-cover state...
		FVector CameraForward = Camera ? Camera->GetForwardVector() : FVector::ForwardVector;
		FVector CameraForwardGrounded = FVector(CameraForward.X, CameraForward.Y, 0.0f).GetSafeNormal();
		
		FVector ActualMovementDirection = (CameraForwardGrounded * CurrentForwardInput) + (MovementDirection * CurrentRightInput);
		ActualMovementDirection = ActualMovementDirection.GetSafeNormal();
		
		FVector TraceStartingPoint = GetActorLocation();
		FVector TraceEndPoint = TraceStartingPoint + (ActualMovementDirection * 100.0f);

		FHitResult HitResult;
		FCollisionQueryParams LineTraceParameters;
		LineTraceParameters.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartingPoint, TraceEndPoint, ECC_Visibility, LineTraceParameters);

		FColor LineColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugLine(GetWorld(), TraceStartingPoint, TraceEndPoint, LineColor, false, 1.0f, 0, 2.0f);

		if (bHit)
		{
			float DotProduct = FVector::DotProduct(HitResult.Normal, ActualMovementDirection);
			
			if (DotProduct < 0.0f)
			{
				if (!bIsInCoverState) 
				{
					CoverWallNormal = HitResult.Normal;
					EnterCoverState();
				}
				FVector SlideDirection = FVector::VectorPlaneProject(ActualMovementDirection, HitResult.Normal);
				AddMovementInput(SlideDirection, InputValue);
				return;
			}
		}
		else
		{
			if (bIsInCoverState)
			{
				CoverWallNormal = FVector::ZeroVector;
				ExitCoverState();
			}
		}
		AddMovementInput(MovementDirection, InputValue);
	}
	else
	{
		CurrentRightInput = 0.0f;
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
	// Don't rotate the character when in cover state
	if (bIsInCoverState)
	{
		return;
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		// Get the character's current velocity
		FVector CurrentVelocity = GetCharacterMovement()->Velocity;
		
		// Project velocity onto the ground plane (ignore Z component)
		FVector GroundVelocity = FVector(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);
		
		// Check if we're moving fast enough to warrant rotation
		if (GroundVelocity.Size() >= 10.0f) // Minimum speed threshold
		{
			// Get the direction of movement
			FVector MovementDirection = GroundVelocity.GetSafeNormal();
			
			// Calculate target rotation based on velocity direction
			FRotator TargetRotation = MovementDirection.Rotation();
			FRotator CurrentRotation = GetActorRotation();
			FRotator NewRotation = FRotator(CurrentRotation.Pitch, TargetRotation.Yaw, CurrentRotation.Roll);
			
			// Store and apply the new rotation
			LastKnownCameraRotation = NewRotation;
			SetActorRotation(NewRotation);
		}
		else
		{
			// No significant movement, maintain previous rotation
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

void ASaoirse::EnterCoverState()
{
	bIsInCoverState = true;
	
	// Only rotate character to face the wall when entering cover
	if (!CoverWallNormal.IsZero())
	{
		// Calculate the direction to face the wall (opposite of wall normal)
		FVector FaceWallDirection = -CoverWallNormal;
		FaceWallDirection.Z = 0.0f; // Keep rotation on horizontal plane only
		FaceWallDirection.Normalize();
		
		// Create rotation from direction
		FRotator TargetRotation = FaceWallDirection.Rotation();
		
		// Set the character's rotation to face the wall immediately when entering cover
		SetActorRotation(TargetRotation);
		
		// Debug visualization
		FVector CharacterLocation = GetActorLocation();
		FVector WallDirection = CharacterLocation + (FaceWallDirection * 100.0f);
		
		DrawDebugLine(GetWorld(), CharacterLocation, WallDirection, FColor::Green, false, 2.0f, 0, 3.0f);
		DrawDebugString(GetWorld(), CharacterLocation + FVector(0, 0, 100), 
			TEXT("Character Facing Wall"), nullptr, FColor::Green, 2.0f, false, 1.5f);
		
		UE_LOG(LogTemp, Log, TEXT("Character rotated to face wall. Wall Normal: %s, Face Direction: %s"), 
			*CoverWallNormal.ToString(), *FaceWallDirection.ToString());
	}
	
	UE_LOG(LogTemp, Log, TEXT("Entered cover state"));
}


void ASaoirse::ExitCoverState()
{
	UE_LOG(LogTemp, Display, TEXT("Exiting cover state"));
	bIsInCoverState = false;

	// Restore default camera settings
	if (SpringCam && Camera && !bIsFirstPersonMode)
	{
		// Restore previous camera settings
		SpringCam->TargetArmLength = DefaultTargetArmLength;
		SpringCam->SetRelativeLocation(DefaultRelativeLocation);
		SpringCam->SetRelativeRotation(DefaultRelativeRotation);
		
		// Re-enable camera lag if it was disabled
		SpringCam->bEnableCameraLag = true;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Exited Cover State - Camera restored to default"));
}

void ASaoirse::MoveForwardCompleted(const FInputActionInstance& Inst)
{
	CurrentForwardInput = 0.0f;
}

void ASaoirse::MoveRightCompleted(const FInputActionInstance& Inst)
{
	CurrentRightInput = 0.0f;
}

void ASaoirse::CameraAdjustToGroundLevel(float DeltaSeconds)
{
	// Smooth camera transitions for cover state
	if (SpringCam && !bIsFirstPersonMode)
	{
		float InterpSpeed = 5.0f; // Adjust for faster/slower transitions
		
		if (bIsInCoverState)
		{
			// Smoothly interpolate to cover camera settings
			float CurrentArmLength = SpringCam->TargetArmLength;
			float NewArmLength = FMath::FInterpTo(CurrentArmLength, CoverTargetArmLength, DeltaSeconds, InterpSpeed);
			SpringCam->TargetArmLength = NewArmLength;
			
			FVector CurrentLocation = SpringCam->GetRelativeLocation();
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, CoverRelativeLocation, DeltaSeconds, InterpSpeed);
			SpringCam->SetRelativeLocation(NewLocation);
			
			// Calculate camera rotation to face the wall
			FRotator TargetCoverRotation = CoverRelativeRotation;
			if (!CoverWallNormal.IsZero())
			{
				// Calculate direction from character to wall (opposite of wall normal)
				FVector WallDirection = -CoverWallNormal;
				WallDirection.Z = 0.0f; // Keep on horizontal plane
				WallDirection.Normalize();
				
				// Convert wall direction to rotation
				FRotator WallRotation = WallDirection.Rotation();
				
				// Set the camera to look at the wall with a slight downward angle
				TargetCoverRotation = FRotator(-10.0f, WallRotation.Yaw, 0.0f);
			}
			
			FRotator CurrentRotation = SpringCam->GetRelativeRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetCoverRotation, DeltaSeconds, InterpSpeed);
			SpringCam->SetRelativeRotation(NewRotation);
		}
		else
		{
			// Smoothly interpolate back to default camera settings
			float CurrentArmLength = SpringCam->TargetArmLength;
			if (!FMath::IsNearlyEqual(CurrentArmLength, DefaultTargetArmLength, 1.0f))
			{
				float NewArmLength = FMath::FInterpTo(CurrentArmLength, DefaultTargetArmLength, DeltaSeconds, InterpSpeed);
				SpringCam->TargetArmLength = NewArmLength;
			}
			
			FVector CurrentLocation = SpringCam->GetRelativeLocation();
			if (!CurrentLocation.Equals(DefaultRelativeLocation, 1.0f))
			{
				FVector NewLocation = FMath::VInterpTo(CurrentLocation, DefaultRelativeLocation, DeltaSeconds, InterpSpeed);
				SpringCam->SetRelativeLocation(NewLocation);
			}
			
			FRotator CurrentRotation = SpringCam->GetRelativeRotation();
			if (!CurrentRotation.Equals(DefaultRelativeRotation, 1.0f))
			{
				FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DefaultRelativeRotation, DeltaSeconds, InterpSpeed);
				SpringCam->SetRelativeRotation(NewRotation);
			}
		}
	}
}

bool ASaoirse::IsWallEdgeDetected(const FVector& Direction, FVector& OutEdgeLocation) const
{
	if (CoverWallNormal.IsZero()) return false;
	
	FVector StartLocation = GetActorLocation();
	
	// Use the movement direction to offset the trace start position
	// This checks for edges ahead of where the character is trying to move

	FVector OffsetStartLocation = StartLocation + (Direction.GetSafeNormal() * 70.0f);

	DrawDebugLine(GetWorld(), StartLocation, OffsetStartLocation, FColor::Green, false, 1.0f, 0, 1.5f);
	
	// Perform multiple traces to detect wall edge
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	
	// Trace 1: Check if wall continues in the movement direction
	FVector TraceStart = OffsetStartLocation;
	FVector TraceEnd = TraceStart + (-CoverWallNormal * 100.0f); // Trace toward where wall should be
	
	FHitResult HitResult;
	bool bWallContinues = GetWorld()->LineTraceSingleByChannel(
		HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
	
	// Debug: Draw the edge detection trace
	FColor LineColor = bWallContinues ? FColor::Blue : FColor::Orange;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 1.0f, 0, 1.5f);
	
	// Trace 2: Check at a slightly higher position to ensure wall continues vertically
	FVector UpperTraceStart = TraceStart + FVector(0, 0, EdgeDetectionHeight);
	FVector UpperTraceEnd = UpperTraceStart + (-CoverWallNormal * 100.0f);
	
	FHitResult UpperHitResult;
	bool bUpperWallContinues = GetWorld()->LineTraceSingleByChannel(
		UpperHitResult, UpperTraceStart, UpperTraceEnd, ECC_Visibility, TraceParams);
	
	// Debug: Draw the upper edge detection trace
	FColor UpperLineColor = bUpperWallContinues ? FColor::Blue : FColor::Orange;
	DrawDebugLine(GetWorld(), UpperTraceStart, UpperTraceEnd, UpperLineColor, false, 1.0f, 0, 1.5f);
	
	// If either trace doesn't hit a wall, we've reached an edge
	bool bEdgeDetected = false;
	FVector EdgeLocation = FVector::ZeroVector;
	
	if (!bWallContinues || !bUpperWallContinues)
	{
		bEdgeDetected = true;
		// Use the trace start position as the edge location
		EdgeLocation = TraceStart;
	}
	
	// Additional check: Ensure the wall normal is similar (same wall)
	if (bWallContinues && bUpperWallContinues)
	{
		float NormalSimilarity = FVector::DotProduct(HitResult.Normal, CoverWallNormal);
		float UpperNormalSimilarity = FVector::DotProduct(UpperHitResult.Normal, CoverWallNormal);
		
		// If the wall normal has changed significantly, we've reached a corner/edge
		if (NormalSimilarity < 0.8f || UpperNormalSimilarity < 0.8f)
		{
			bEdgeDetected = true;
			// Use the hit location where the normal changed
			EdgeLocation = HitResult.Location;
		}
	}
	
	if (bEdgeDetected)
	{
		OutEdgeLocation = EdgeLocation;
		// Debug visualization
		DrawDebugSphere(GetWorld(), EdgeLocation, 25.0f, 8, FColor::Red, false, 2.0f, 0, 3.0f);
		DrawDebugString(GetWorld(), EdgeLocation + FVector(0, 0, 50), 
			TEXT("WALL EDGE"), nullptr, FColor::White, 2.0f, false, 1.5f);
	}
	
	return bEdgeDetected;
}

