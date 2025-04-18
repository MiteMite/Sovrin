#include "Saoirse.h"

ASaoirse::ASaoirse()
{
	
}

void ASaoirse::Tick(float DeltaSeconds)
{
	
}

void ASaoirse::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	//Input->BindAction(IA_Walk,ETriggerEvent::Triggered,PlayerInputComponent);
	
}

void ASaoirse::MoveForward(const FInputActionInstance& Inst)
{
	
}

ASaoirse::~ASaoirse()
{
	
}

