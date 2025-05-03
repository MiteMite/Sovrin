// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/RingBuffer.h"
#include "GameFramework/GameModeBase.h"
#include "kismet/GameplayStatics.h"
#include "TimeTravel.h"
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
	virtual void BeginPlay() override;
	FOnTimeTravelStarted OnTimeTravelStarted;
	FOnTimeTravelEnded OnTimeTravelEnded;

protected:
	virtual ~ATimeTravelGlobal() override; //destructor*/
};
