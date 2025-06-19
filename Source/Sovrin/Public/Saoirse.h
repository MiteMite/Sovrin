#pragma once

#include "CoreMinimal.h"
#include "Consumable.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Equipment.h"
#include "InputMappingContext.h"
#include "TimeTravel.h"
#include "TimeTravelGlobal.h"
#include "Weapon.h"
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
	float DefaultTargetArmLength = 900.0f;										//the camera is 900 units away from the character
	FVector DefaultRelativeLocation = FVector(0.0f, 0.0f, 50.0f);	//the camera is 50 units above the character
	FRotator DefaultRelativeRotation = FRotator(-60.0f, 180.0f, 0.0f); //Looks down 60 degrees, and is rotated 180 degrees around the Y axis
	FRotator LastKnownCameraRotation = FRotator::ZeroRotator;
	
	// Cover camera settings
	float CoverTargetArmLength = 300.0f;  // Closer to character in cover
	FVector CoverRelativeLocation = FVector(0.0f, 0.0f, 20.0f);  // Lower, near ground level
	FRotator CoverRelativeRotation = FRotator(-10.0f, 180.0f, 0.0f);  // Nearly horizontal, looking at character
	
	//Cover camera functions
	void CameraAdjustToGroundLevel(float DeltaSeconds);

	//Mouse sensitivity settings
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float MouseSensitivity = 1.0f;

	//Pitch limits for first-person camera
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "-90", ClampMax = "0.0"))
	float FirstPersonPitchMin = -90.0f;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0", ClampMax = "90"))
	float FirstPersonPitchMax = 90.0f;

	//Input variables
	bool bIsInCoverState = false; //tracks if a character is currently in cover
	float CurrentForwardInput = 0.0f;
	float CurrentRightInput = 0.0f;
	FVector CoverWallNormal = FVector::ZeroVector; // Store the wall normal when in cover
	float CoverExitThreshold = 0.85f; // How perpendicular movement must be to exit cover (0.85 = ~32 degree cone)
	
	// Edge detection parameters
	float EdgeDetectionDistance = 50.0f; // How far ahead to check for wall edges
	float EdgeDetectionHeight = 50.0f; // Height offset for edge detection traces
	FVector LastDetectedEdgeLocation = FVector::ZeroVector; // Store the last detected edge location
	bool bIsMovementBlockedByEdge = false; // Track if the edge currently blocks movement
	FVector EdgeDetectionDirection = FVector::ZeroVector; // Direction where edge was detected
	float EdgeCameraOffsetDistance = 200.0f; // How much to offset the camera when edge is detected
	float EdgeCameraLookAheadDistance = 200.0f; // How far ahead the camera should look when edge is detected
	float EdgeCameraTransitionSpeed = 8.0f; // Speed of camera transition when edge is detected/lost
	
	// Edge detection function
	UFUNCTION()
	bool IsWallEdgeDetected(const FVector& Direction, FVector& OutEdgeLocation) const;

	//Input functions
	void MoveForward(const FInputActionInstance& Inst);
	void MoveRight(const FInputActionInstance& Inst);
	void CrouchProne(const FInputActionInstance& Inst);
	void RewindTime(const FInputActionInstance& Inst);
	void StartFirstPersonMode(const FInputActionInstance& Inst);
	void StopFirstPersonMode(const FInputActionInstance& Inst);
	void FirstPersonLook(const FInputActionInstance& Inst);
	void EnterCoverState();
	void ExitCoverState();
	UFUNCTION()
	void TogglePauseMenu(const FInputActionInstance& Inst);
	void ToggleInventory(const FInputActionInstance& Inst);
	void MoveForwardCompleted(const FInputActionInstance& Inst);
	void MoveRightCompleted(const FInputActionInstance& Inst);

	//Inventory variables
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<AWeapon*> Weapons;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<AConsumable*> Consumables;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<AEquipment*> Equipment;
	
	
protected:

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UEnhancedInputComponent* Input;
	
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

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputPauseMenu;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InputInventory;
};
