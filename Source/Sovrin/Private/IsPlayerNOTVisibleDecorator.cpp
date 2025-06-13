// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerNOTVisibleDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UIsPlayerNOTVisibleDecorator::UIsPlayerNOTVisibleDecorator() : Super()
{
	NodeName = TEXT("Is Player NOT Visible");
	bAllowAbortChildNodes = true;
	FlowAbortMode = EBTFlowAbortMode::Self;
	BlackboardKey.SelectedKeyName = TEXT("IsPlayerVisible");
	BlackboardKey.SelectedKeyType = UBlackboardKeyType_Bool::StaticClass();
	IsPlayerNOTVisibleKey = BlackboardKey;
	NotifyObserver = EBTBlackboardRestart::ValueChange;
}

bool UIsPlayerNOTVisibleDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	
	if (!BlackboardComponent)
		return false;
	
	return !BlackboardComponent->GetValueAsBool("IsPlayerVisible");
}
