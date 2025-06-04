#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enhancedinputcomponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "TimeTravel.h"
#include "TimeTravelGlobal.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "animation/AnimInstance.h"
#include "animation/AnimBlueprint.h"
#include "animation/AnimBlueprintGeneratedClass.h"
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
	ATimeTravelGlobal* TimeTravelGameMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTimeTravel* TimeTravelComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionStimuliSourceComponent* StimuliSource;
private:
	UFUNCTION()
	void UpdateRotationBasedOnMovement();
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category = "Components")
	USpringArmComponent* SpringCam;
	UPROPERTY(VisibleAnywhere, Blueprintable, Category = "Components")
	UCameraComponent* Camera;

	//Camera system variables
	bool bIsFirstPersonMode = false;
	float DefaultTargetArmLength = 900.0f;										//camera is 900 units away from the character
	FVector DefaultRelativeLocation = FVector(0.0f, 0.0f, 50.0f);	//camera is 50 units above the character
	FRotator DefaultRelativeRotation = FRotator(-60.0f, 180.0f, 0.0f); //Looks down 60 degrees, and is rotated 180 degrees around the Y axis
	FRotator LastKnownCameraRotation = FRotator::ZeroRotator;

	//Mouse sensitivity settings
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float MouseSensitivity = 1.0f;

	//Pitch limits for first person camera
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "-90", ClampMax = "0.0"))
	float FirstPersonPitchMin = -90.0f;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0", ClampMax = "90"))
	float FirstPersonPitchMax = 90.0f;

	//Input functions
	void MoveForward(const FInputActionInstance& Inst);
	void MoveRight(const FInputActionInstance& Inst);
	void CrouchProne(const FInputActionInstance& Inst);
	void RewindTime(const FInputActionInstance& Inst);
	void StartFirstPersonMode(const FInputActionInstance& Inst);
	void StopFirstPersonMode(const FInputActionInstance& Inst);
	void FirstPersonLook(const FInputActionInstance& Inst);
	
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

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputFirstPersonToggle;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputLook;
};
