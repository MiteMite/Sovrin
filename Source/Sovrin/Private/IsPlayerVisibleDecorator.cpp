// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerVisibleDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UIsPlayerVisibleDecorator::UIsPlayerVisibleDecorator()
{
	NodeName = TEXT("Is Player Visible");
	bAllowAbortChildNodes = true;
	FlowAbortMode = EBTFlowAbortMode::LowerPriority; //Abort patrol when player is visible
	BlackboardKey.SelectedKeyName = TEXT("IsPlayerVisible");
	BlackboardKey.SelectedKeyType = UBlackboardKeyType_Bool::StaticClass();
	IsPlayerVisibleKey = BlackboardKey;
	NotifyObserver = EBTBlackboardRestart::ValueChange; //react to blackboard changes
	bNotifyBecomeRelevant = true; //notify when blackboard key becomes relevant
	bNotifyCeaseRelevant = true; //notify when blackboard key becomes irrelevant
}

bool UIsPlayerVisibleDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
		return false;
	
	bool bResult = BlackboardComponent->GetValueAsBool("IsPlayerVisible");
	//UE_LOG(LogTemp, Warning, TEXT("IsPlayerVisible Decorator: %s"), bResult ? TEXT("TRUE") : TEXT("FALSE"));

	return bResult;
}