// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "AbortableWaitTask.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UAbortableWaitTask : public UBTTask_Wait
{
	GENERATED_BODY()
	UAbortableWaitTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsPlayerVisibleKey;
};
