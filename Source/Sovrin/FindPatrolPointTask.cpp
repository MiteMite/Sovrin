// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPatrolPointTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseNME.h"

UFindPatrolPointTask::UFindPatrolPointTask()
{
	NodeName = TEXT("Find Patrol Point");
}

EBTNodeResult::Type UFindPatrolPointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	//UE_LOG(LogTemp, Warning, TEXT("Executing find patrol point task"));
	if (!AIController && !BlackboardComponent )
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Ai controller and blackboard component"));
		return EBTNodeResult::Failed;
	}
	else
	{
		FVector NewLocation = AIController->GetBlackboardComponent()->GetValueAsVector("PatrolPointLocation");
		BlackboardComponent->SetValueAsVector("PatrolPointLocation", NewLocation);
		if (OwnerComp.GetOwner()->IsA(ABaseNMEai::StaticClass()))
		{
			OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
}

void UFindPatrolPointTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	UE_LOG(LogTemp, Warning, TEXT("Find Patrol Point Task Finished"));
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (TaskResult == EBTNodeResult::Succeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving to patrol point %s"), *BlackboardComponent->GetValueAsVector("PatrolPointLocation").ToString());
		AIController->MoveToLocation(BlackboardComponent->GetValueAsVector("PatrolPointLocation"));
	}
	if (TaskResult != EBTNodeResult::Succeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find patrol point"));
	}
}
