﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FindPatrolPointTask.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UFindPatrolPointTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UFindPatrolPointTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;
};
