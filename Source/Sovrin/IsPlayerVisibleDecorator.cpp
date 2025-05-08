// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerVisibleDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UIsPlayerVisibleDecorator::UIsPlayerVisibleDecorator()
{
	NodeName = TEXT("Is Player Visible");
}

bool UIsPlayerVisibleDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
		return false;

	return BlackboardComponent->GetValueAsObject(PlayerKey.SelectedKeyName)!=nullptr;
}