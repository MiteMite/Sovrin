#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/RingBuffer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "TimeTravel.generated.h"

/**
 * Component recording the state of the specific
 * game object at the moment a snapshot is
 * taken and the state of the timeline.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeTravelStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeTravelEnded);

USTRUCT()
struct FTransformAndVelocitySnapshot
{
	GENERATED_BODY()
	float TimeSinceLastSnapshot = 0.0f;
	FTransform Transform{FVector::ZeroVector};
	FVector LinearVelocity = FVector::ZeroVector;
	FVector AngularVelocityInRadians = FVector::ZeroVector;
};
USTRUCT()
struct FMovementVelocityAndModeSnapshot
{
	GENERATED_BODY()
	float TimeSinceLastSnapshot = 0.0f;
	FVector MovementVelocity = FVector::ZeroVector;
	TEnumAsByte<enum EMovementMode> MovementMode = EMovementMode::MOVE_None;
};

UCLASS()
class SOVRIN_API UTimeTravel : public UActorComponent
{
	GENERATED_BODY()
public:
	UTimeTravel(); //constructor
	virtual void BeginPlay() override;
	bool IsRewinding = false;		//Currently rewinding time
	bool IsFastForwarding = false;	//currently fast forwarding time
	bool IsTimeScrubbing = false; //currently moving the timeline in any direction
	float PlaybackSpeed = 1.0f;	//speed of playback
	void RecordSnapshot(float DeltaTime);	//Function to record current transform
	void PlaySnapshots(float DeltaTime, bool bRewinding); //Play recorded snapshots at a certain rate of time
	void PauseTime(float DeltaTime, bool bRewinding); //Pause all movement in the game
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FOnTimeTravelStarted OnTimeTravelStarted;
	UFUNCTION()
	void FOnTimeTravelStarted(); //begin state
	FOnTimeTravelEnded OnTimeTravelEnded;
	UFUNCTION()
	void FOnTimeTravelEnded();	//end state
private:
	
	UPrimitiveComponent* OwnerRootComponent;
	UCharacterMovementComponent* OwnerMovementComponent; 
	USkeletalMeshComponent* OwnerSkeletalMeshComponent;
	TRingBuffer<FTransformAndVelocitySnapshot> TransformAndVelocitySnapshots;
	TRingBuffer<FMovementVelocityAndModeSnapshot> MovementVelocityAndModeSnapshots;
protected:
	virtual ~UTimeTravel() override; //destructor
};

