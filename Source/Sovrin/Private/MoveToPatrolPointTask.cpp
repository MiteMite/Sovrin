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
	UE_LOG(LogTemp, Warning, TEXT("Executing move to patrol point task with location: %s"), *PatrolPointLocation.ToString());
	AIController->MoveToLocation(PatrolPointLocation);
	OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

void UMoveToPatrolPointTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	ABaseNMEai* NMEAI = Cast<ABaseNMEai>(AIController);
	if (NMEAI)
	{
		FVector NewLocation = NMEAI->GetNextPatrolPoint()->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Next patrol point %s"), *NewLocation.ToString());
		BlackboardComponent->SetValueAsVector("PatrolPointLocation", NewLocation);
	}
}
