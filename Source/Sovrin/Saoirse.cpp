#include "Sovrin/Saoirse.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TimeTravel.h"
#include "ToolBuilderUtil.h"
#include "Kismet/GameplayStatics.h"


ASaoirse::ASaoirse()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextObject(TEXT("/Game/SovrinClasses/InputMapping/IMC_Base.IMC_Base"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputForwardObject(TEXT("/Game/SovrinClasses/InputMapping/IA_MoveForward.IA_MoveForward"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRightObject(TEXT("/Game/SovrinClasses/InputMapping/IA_MoveRight.IA_MoveRight"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRewindObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Rewind.IA_Rewind"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));

	InputMapping = InputMappingContextObject.Object;
	InputForwardAction = UInputForwardObject.Object;
	InputRightAction = UInputRightObject.Object;
	InputRewind = UInputRewindObject.Object;
	this->GetMesh()->SetSkeletalMeshAsset(USkeletalMeshObject.Object);
	this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
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
}

void ASaoirse::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASaoirse::BeginPlay()
{
	Super::BeginPlay();
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

void ASaoirse::RewindTime(const FInputActionInstance& Inst)
{
	if (Inst.GetTriggerEvent()==ETriggerEvent::Started)
	{
		
		if (TimeTravelComponent!=nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Started Rewinding time"));
			TimeTravelComponent->OnTimeTravelStarted.Broadcast();
		}
	}
	if (Inst.GetTriggerEvent()==ETriggerEvent::Completed)
	{
		if (TimeTravelComponent!=nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Stopped Rewinding time"));
			TimeTravelComponent->OnTimeTravelEnded.Broadcast();
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("My Current Transform is %s"),*GetTransform().ToString());
}

ASaoirse::~ASaoirse()
{
	
}

