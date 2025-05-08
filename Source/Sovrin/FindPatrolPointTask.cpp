// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPatrolPointTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UFindPatrolPointTask::UFindPatrolPointTask()
{
	NodeName = TEXT("Find Patrol Point");
}

EBTNodeResult::Type UFindPatrolPointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!AIController && !BlackboardComponent)
		return EBTNodeResult::Failed;

	FVector NewLocation = AIController->GetPawn()->GetActorLocation();

	BlackboardComponent->SetValueAsVector(TargetLocationKey.SelectedKeyName, NewLocation);
	UE_LOG(LogTemp, Warning, TEXT("New location is: %s"), *NewLocation.ToString());
	return EBTNodeResult::Succeeded;
}