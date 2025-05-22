// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayerTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UChasePlayerTask::UChasePlayerTask()
{
	NodeName = "Chase Player";

	PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UChasePlayerTask, PlayerLocationKey));
}

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	FVector PlayerLocation = BlackboardComponent->GetValueAsVector(PlayerLocationKey.SelectedKeyName);
	if (PlayerLocation.IsZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player location is found"));
		return EBTNodeResult::Failed;
	}

	AIController->MoveToLocation(PlayerLocation);
	UE_LOG(LogTemp, Display, TEXT("Chasing player at location: %s"), *PlayerLocation.ToString());
	return EBTNodeResult::Succeeded;
}