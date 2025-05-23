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

	if (!AIController && !BlackboardComponent )
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Ai controller and blackboard component"));
		return EBTNodeResult::Failed;
	}
	else
	{
		if (AIController->GetBlackboardComponent()->GetValueAsBool("IsPlayerVisible")==false)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Executing Find Patrol Point Task"));
			FVector NewLocation = AIController->GetBlackboardComponent()->GetValueAsVector("PatrolPointLocation");
			BlackboardComponent->SetValueAsVector("PatrolPointLocation", NewLocation);
			if (OwnerComp.GetOwner()->IsA(ABaseNMEai::StaticClass()))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Moving to patrol point %s"), *NewLocation.ToString());
				AIController->MoveToLocation(NewLocation);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				return EBTNodeResult::Failed;
			}
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
	
	ExecuteTask(OwnerComp, NodeMemory);
}
