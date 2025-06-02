// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPatrolPointTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Sovrin/Public/BaseNME.h"
#include "Navigation/PathFollowingComponent.h"

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
		if (AIController->IsA(ABaseNMEai::StaticClass()))
		{
			AIController->GetBlackboardComponent()->SetValueAsVector("PatrolPointLocation", Cast<ABaseNMEai>(AIController)->GetCurrentPatrolPoint()->GetActorLocation());
			FVector NewLocation = AIController->GetBlackboardComponent()->GetValueAsVector("PatrolPointLocation");
			BlackboardComponent->SetValueAsVector("PatrolPointLocation", NewLocation);
			//UE_LOG(LogTemp, Warning, TEXT("Patrol Point Location: %s"), *NewLocation.ToString());
			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
}
