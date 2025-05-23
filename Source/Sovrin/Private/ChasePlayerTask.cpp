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
	//UE_LOG(LogTemp, Display, TEXT("Executing Chase Player Task"));
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

	//UE_LOG(LogTemp, Warning, TEXT("Moving to player location %s"), *PlayerLocation.ToString());
	AIController->MoveToLocation(PlayerLocation);
	return EBTNodeResult::Succeeded;

}
