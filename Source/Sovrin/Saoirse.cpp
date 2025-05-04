#include "Sovrin/Saoirse.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TimeTravelGlobal.h"
#include "GameFramework/GameModeBase.h"
#include "Sovrin/TimeTravel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"


ASaoirse::ASaoirse()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextObject(TEXT("/Game/SovrinClasses/InputMapping/IMC_Base.IMC_Base"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputForwardObject(TEXT("/Game/SovrinClasses/InputMapping/IA_MoveForward.IA_MoveForward"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRightObject(TEXT("/Game/SovrinClasses/InputMapping/IA_MoveRight.IA_MoveRight"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRewindObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Rewind.IA_Rewind"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputCrouchObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Crouch.IA_Crouch"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> UAnimationClass(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny"));

	this->GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	InputMapping = InputMappingContextObject.Object;
	InputForwardAction = UInputForwardObject.Object;
	InputRightAction = UInputRightObject.Object;
	InputRewind = UInputRewindObject.Object;
	InputCrouch = UInputCrouchObject.Object;
	this->GetMesh()->SetSkeletalMeshAsset(USkeletalMeshObject.Object);
	this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	this->GetCharacterMovement()->GravityScale = 20.0f;
	this->GetCharacterMovement()->MaxWalkSpeedCrouched = 150.0f;
	TimeTravelComponent = CreateDefaultSubobject<UTimeTravel>(TEXT("TimeTravel"));
	// Create the CameraBoom (SpringArmComponent)
	SpringCam = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringCam->SetupAttachment(this->GetRootComponent());

	// Configure the CameraBoom properties
	SpringCam->TargetArmLength = 900.0f; // Set the distance between the camera and the pawn
	SpringCam->bUsePawnControlRotation = true; // Let the boom follow the pawn's control rotation
	SpringCam->bDoCollisionTest = false; // Don't want to collide with the pawn
	SpringCam->bEnableCameraLag = true; // Enable camera lag
	SpringCam->CameraLagSpeed = 10.0f; // Set the camera lag speed
	SpringCam->CameraLagMaxDistance = 1000.0f; // Set the maximum distance the camera can lag
	SpringCam->bUsePawnControlRotation = false; // Let the boom follow the pawn's control rotation
	// Create the CameraComponent
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringCam); // Attach the camera to the CameraBoom
	// Configure the CameraComponent
	Camera->bUsePawnControlRotation = false; // Do not rotate the camera with the pawn
	SpringCam->SetRelativeRotation(FRotator(-80.0f, 0.0f, 0.0f));

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
	//adjust rotation
	if (!this->GetVelocity().IsNearlyZero() || this->GetCharacterMovement()->IsFalling())
	{
		FRotator Rotation = this->GetVelocity().Rotation()+FRotator(0.0f,90.0f,0.0f);
		this->GetMesh()->SetRelativeRotation(FRotator(0.0f,Rotation.Yaw,Rotation.Roll));
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
	}
}

void ASaoirse::MoveForward(const FInputActionInstance& Inst)
{
	this->GetMovementComponent()->AddInputVector(FVector::ForwardVector*Inst.GetValue().Get<float>());
}
void ASaoirse::MoveRight(const FInputActionInstance& Inst)
{
	this->GetMovementComponent()->AddInputVector(Inst.GetValue().Get<float>()*FVector::RightVector);
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

ASaoirse::~ASaoirse()
{
	
}

