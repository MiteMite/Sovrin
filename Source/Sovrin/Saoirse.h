#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enhancedinputcomponent.h"
#include "Saoirse.generated.h"

UCLASS()
class SOVRIN_API ASaoirse : public ACharacter
{
public:
	GENERATED_BODY()
	
	ASaoirse();
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(const FInputActionInstance& Inst);
	
protected:
	virtual ~ASaoirse() override;
};
