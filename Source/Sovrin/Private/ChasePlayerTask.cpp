// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayerTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Sovrin/BaseNMEai.h"
#include "Navigation/PathFollowingComponent.h"

UChasePlayerTask::UChasePlayerTask()
{
	NodeName = "Chase Player";

	PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UChasePlayerTask, PlayerLocationKey));
	
}

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedAIController = OwnerComp.GetAIOwner();
	if (!CachedAIController)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	FVector PlayerLocation = BlackboardComponent->GetValueAsVector("PlayerLocation");

	//Start the movement
	EPathFollowingRequestResult::Type MoveResult = CachedAIController->MoveToLocation(PlayerLocation);

	if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chase Player Task - Movement Started"));
		return EBTNodeResult::InProgress;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Chase Player Task - Movement Failed"));
		return EBTNodeResult::Failed;
	}
}

void UChasePlayerTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (CachedAIController)
	{
		UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
		if (BlackboardComponent)
		{
			FVector NewPlayerLocation = BlackboardComponent->GetValueAsVector("PlayerLocation");
			CachedAIController->MoveToLocation(NewPlayerLocation);
		}

	}
}


void UChasePlayerTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (CachedAIController)
	{
		CachedAIController->StopMovement();
		UE_LOG(LogTemp, Warning, TEXT("Chase Player Task - Movement Stopped"));
	}
	CachedAIController = nullptr;
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
