// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "AIController.h"
#include "MoveToPatrolPointTask.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UMoveToPatrolPointTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMoveToPatrolPointTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	FBlackboardKeySelector GetTargetLocationKey() const { return TargetLocationKey; }

private:
	FVector PatrolPointLocation;
protected:
	FBlackboardKeySelector TargetLocationKey;
};
