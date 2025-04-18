#include "Sovrin/Saoirse.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/Iris/ReplicationSystem/ReplicationSystemUtil.h"

ASaoirse::ASaoirse()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextObject(TEXT("/Game/SovrinClasses/InputMapping/IMC_Base.IMC_Base"));
	static ConstructorHelpers::FObjectFinder<UInputAction> UInputActionObject(TEXT("/Game/SovrinClasses/InputMapping/IA_Walk.IA_Walk"));
	InputMapping=InputMappingContextObject.Object;
	InputAction = UInputActionObject.Object;
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
	}
	
}

void ASaoirse::MoveForward(const FInputActionInstance& Inst)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MoveForward");
	UE_LOG(LogTemp, Display, TEXT("MoveForward"));
}

ASaoirse::~ASaoirse()
{
	
}

