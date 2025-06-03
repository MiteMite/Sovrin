// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPatrolPointTask.h"
#include "Sovrin/Public/BaseNMEai.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UMoveToPatrolPointTask::UMoveToPatrolPointTask()
{
	NodeName = TEXT("Move To Patrol Point");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UMoveToPatrolPointTask, TargetLocationKey));
}

EBTNodeResult::Type UMoveToPatrolPointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedAIController = OwnerComp.GetAIOwner();

	PatrolPointLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("PatrolPointLocation");
	//Start the movement
	EPathFollowingRequestResult::Type MoveResult = CachedAIController->MoveToLocation(PatrolPointLocation);

	if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Move to patrol point Task - Movement Started"));
		return EBTNodeResult::InProgress;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Move to patrol point Task - Movement Failed"));
		return EBTNodeResult::Failed;
	}
}

void UMoveToPatrolPointTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (CachedAIController)
	{
		UPathFollowingComponent* PathFollowingComponent = CachedAIController->GetPathFollowingComponent();
		if (PathFollowingComponent && PathFollowingComponent->GetStatus()!= EPathFollowingStatus::Moving)
		{
			if (PathFollowingComponent->GetStatus() == EPathFollowingStatus::Idle)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			}
		}
	}
}

void UMoveToPatrolPointTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (CachedAIController)
	{
		CachedAIController->StopMovement();
		//UE_LOG(LogTemp, Warning, TEXT("Move to patrol point Task - Movement Stopped"));
		Cast<ABaseNMEai>(CachedAIController)->GetNextPatrolPoint();
		CachedAIController->GetBlackboardComponent()->SetValueAsVector("PatrolPointLocation", PatrolPointLocation);
	}
	CachedAIController = nullptr;
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
