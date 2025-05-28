// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPatrolPointTask.h"
#include "Sovrin/BaseNMEai.h"

UMoveToPatrolPointTask::UMoveToPatrolPointTask()
{
	NodeName = TEXT("Move To Patrol Point");
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UMoveToPatrolPointTask, TargetLocationKey));
}

EBTNodeResult::Type UMoveToPatrolPointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	PatrolPointLocation = BlackboardComponent->GetValueAsVector("PatrolPointLocation");

	if (OwnerComp.GetOwner()->GetActorLocation()!=PatrolPointLocation)
	{
		AIController->MoveToLocation(PatrolPointLocation);
		return EBTNodeResult::Failed;
	}
	else
	{
		return EBTNodeResult::Succeeded;
	}

}
void UMoveToPatrolPointTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	PatrolPointLocation = BlackboardComponent->GetValueAsVector("PatrolPointLocation");
	if (AIController->GetOwner()->GetActorLocation() == PatrolPointLocation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Patrol point reached"));
		if (ABaseNMEai* ParentBaseNME = Cast<ABaseNMEai>(OwnerComp.GetOwner()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Getting next patrol point"))
			ParentBaseNME->GetNextPatrolPoint();
			AIController->GetBlackboardComponent()->SetValueAsVector("PatrolPointLocation", ParentBaseNME->GetCurrentPatrolPoint()->GetActorLocation());
		}
	}
}
