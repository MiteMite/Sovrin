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
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!AIController && !BlackboardComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Ai controller and blackboard component"));
		return EBTNodeResult::Failed;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("My Owner is: %s"), *OwnerComp.GetAIOwner()->GetName());
		FVector NewLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName);
		BlackboardComponent->SetValueAsVector(TargetLocationKey.SelectedKeyName, NewLocation);
		//UE_LOG(LogTemp, Warning, TEXT("New location is: %s"), *NewLocation.ToString());
		return EBTNodeResult::Succeeded;
	}

}