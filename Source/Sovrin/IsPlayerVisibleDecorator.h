// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "IsPlayerVisibleDecorator.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UIsPlayerVisibleDecorator : public UBTDecorator
{
	GENERATED_BODY()
public:
	UIsPlayerVisibleDecorator();
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsPlayerVisibleKey;
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


};
