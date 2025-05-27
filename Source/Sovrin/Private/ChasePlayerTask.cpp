// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayerTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Sovrin/BaseNMEai.h"

UChasePlayerTask::UChasePlayerTask()
{
	NodeName = "Chase Player";

	PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UChasePlayerTask, PlayerLocationKey));
	
}

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//UE_LOG(LogTemp, Warning, TEXT("Executing chase player task"));
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	FVector PlayerLocation = BlackboardComponent->GetValueAsVector("PlayerLocation");
	if (PlayerLocation.IsZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player location is found"));
		return EBTNodeResult::Failed;
	}
	AIController = OwnerComp.GetAIOwner();
	if (BlackboardComponent->GetValueAsBool("IsPlayerVisible"))
	{
		OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	
}

void UChasePlayerTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	UE_LOG(LogTemp, Warning, TEXT("Chase Player Task Finished"));
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Ai controller"));
		return;
	}
	if (!BlackboardComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load blackboard component"));
		return;
	}
	
	if (TaskResult == EBTNodeResult::Succeeded)
	{
		FVector PlayerLocation = BlackboardComponent->GetValueAsVector("PlayerLocation");
		UE_LOG(LogTemp, Warning, TEXT("Moving to player location %s"),*PlayerLocation.ToString());
		AIController->MoveToLocation(PlayerLocation);
	}
	if (TaskResult != EBTNodeResult::Succeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to chase player"));
	}
}
