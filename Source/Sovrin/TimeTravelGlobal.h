// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/RingBuffer.h"
#include "GameFramework/GameModeBase.h"
#include "TimeTravelGlobal.generated.h"

/**
 * While time travel works on a single game object
 * This class works at making the effect global
 */
USTRUCT()
struct FGlobalTransformAndVelocitySnapshot
{
	GENERATED_BODY()
	FTransform LocalActorTransform;
	
	float LocalActorVelocity;
};
USTRUCT()
struct FGlobalMovementVelocityAndModeSnapshot
{
	GENERATED_BODY()
	
};
UCLASS()
class SOVRIN_API ATimeTravelGlobal : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATimeTravelGlobal();
	bool IsGlobalRewinding();		//Currently rewinding time
	bool IsGlobalFastForward();	//currently  Fast forwarding time
	bool IsGlobalTimeScrubbing(); //currently moving the timeline in any direction
	void RecordSnapshotGlobal();	//Function to record current transform
	void PlaySnapshotsGlobal(float DeltaTime, bool bRewinding); //Play recorded snapshots at a certain rate of time
	void PauseTimeGlobal(float DeltaTime, bool bRewinding); //Pause all movement in the game
	
	
private:
	void FGlobalOnTimeTravelStarted(); //begin state
	void FGlobalOnTimeTravelEnded();	//end state
	TRingBuffer<FGlobalTransformAndVelocitySnapshot> GlobalTransformAndVelocitySnapshots;
	TRingBuffer<FGlobalMovementVelocityAndModeSnapshot> GlobalMovementVelocityAndModeSnapshots;

protected:
	virtual ~ATimeTravelGlobal() override; //destructor*/
};
