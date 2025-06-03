// Fill out your copyright notice in the Description page of Project Settings.


#include "AbortableWaitTask.h"
#include "BehaviorTree/BlackboardComponent.h"
UAbortableWaitTask::UAbortableWaitTask()
{
	NodeName = TEXT("Abortable Wait");
	//Just needed this to make the task tickable
	INIT_TASK_NODE_NOTIFY_FLAGS();
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	WaitTime = 3.0f;
}

EBTNodeResult::Type UAbortableWaitTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check immediately if player is visible before starting wait
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent && BlackboardComponent->GetValueAsBool("IsPlayerVisible"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Wait task - player visible on execute, aborting immediately"));
		return EBTNodeResult::Failed;
	}
    
	//UE_LOG(LogTemp, Warning, TEXT("Wait task started"));
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UAbortableWaitTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Check every tick if player becomes visible
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent && BlackboardComponent->GetValueAsBool("IsPlayerVisible"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Wait task aborted - player visible during tick"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
    
	// Continue normal wait behavior
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UAbortableWaitTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Wait task finished with result: %d"), (int32)TaskResult);
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
