// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "IsPlayerNOTVisibleDecorator.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UIsPlayerNOTVisibleDecorator : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

	
public:
	UIsPlayerNOTVisibleDecorator();
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsPlayerNOTVisibleKey;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
