// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Composites/BTComposite_SimpleParallel.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "Sovrin/Detectplayerservice.h"
#include "BaseBehaviorTree.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UBaseBehaviorTree : public UBehaviorTree
{
	GENERATED_BODY()
public:
	UBaseBehaviorTree();

	UFUNCTION()
	UBlackboardComponent* GetBlackboardComponent();
	UFUNCTION()
	UBlackboardData* GetBlackboardData();
	UFUNCTION()
	UBehaviorTreeComponent* GetBehaviorTreeComponent();
	UFUNCTION()
	UBehaviorTree* GetBehaviorTree();
	UFUNCTION()
	void SetBaseBlackboardComponent(UBlackboardComponent* BlackboardComponent);
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
	UBlackboardComponent* BaseBlackboardComponent;
	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
	UBlackboardData* BaseBlackboardData;
	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
	UBehaviorTreeComponent* BaseBehaviorTreeComponent;
	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
	UBehaviorTree* BaseBehaviorTree;
};
