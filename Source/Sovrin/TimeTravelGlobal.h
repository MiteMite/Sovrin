// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimeTravel.h"
#include "GameFramework/GameModeBase.h"
#include "Delegates/Delegate.h"
#include "TimeTravelGlobal.generated.h"

/**
 * While time travel works on a single game object
 * This class works at making the effect global
 */

UCLASS()
class SOVRIN_API ATimeTravelGlobal : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATimeTravelGlobal();
	virtual void BeginPlay() override;
	TArray<AActor*> ActorsInWorld;
	TArray<UTimeTravel*> TimeTravelActorsInWorld;
	
protected:
	virtual ~ATimeTravelGlobal() override; //destructor
};
