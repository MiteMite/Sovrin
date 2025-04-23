#include "Sovrin/Saoirse.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TimeTravel.h"

ASaoirse::ASaoirse()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextObject(TEXT("/Game/SovrinClasses/InputMapping/IMC_Base.IMC_Base"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputActionObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Walk.IA_Walk"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputRewindObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Rewind.IA_Rewind"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));

	InputMapping = InputMappingContextObject.Object;
	InputAction = UInputActionObject.Object;
	InputRewind = UInputRewindObject.Object;
	this->GetMesh()->SetSkeletalMeshAsset(USkeletalMeshObject.Object);
	TimeTravelComponent = CreateDefaultSubobject<UTimeTravel>(TEXT("TimeTravel"));
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
		Input->BindAction(InputAction,ETriggerEvent::Triggered, this, &ASaoirse::MoveForward);
		Input->BindAction(InputRewind,ETriggerEvent::Started, this, &ASaoirse::RewindTime);
		Input->BindAction(InputRewind,ETriggerEvent::Completed, this, &ASaoirse::RewindTime);
	}
	
}

void ASaoirse::MoveForward(const FInputActionInstance& Inst)
{
	//UE_LOG(LogTemp, Display, TEXT("MoveForward"));
	this->GetMovementComponent()->AddInputVector(Inst.GetValue().Get<FVector>());
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

