#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enhancedinputcomponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "TimeTravel.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Saoirse.generated.h"

UCLASS()
class SOVRIN_API ASaoirse : public ACharacter
{
public:
	GENERATED_BODY()
	
	ASaoirse();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTimeTravel* TimeTravelComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionStimuliSourceComponent* StimuliSource;
private:
	USpringArmComponent* SpringCam;
	UCameraComponent* Camera;
	void MoveForward(const FInputActionInstance& Inst);
	void MoveRight(const FInputActionInstance& Inst);
	void CrouchProne(const FInputActionInstance& Inst);
	void RewindTime(const FInputActionInstance& Inst);
	
protected:
	virtual ~ASaoirse() override;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputForwardAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputRightAction;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputRewind;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputCrouch;
};
