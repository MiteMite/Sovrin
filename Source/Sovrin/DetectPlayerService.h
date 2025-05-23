// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "DetectPlayerService.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UDetectPlayerService : public UBTService
{
	GENERATED_BODY()

public:	
	UDetectPlayerService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void CheckPlayerDetection(UBehaviorTreeComponent OwnerComp);
	AAIController* AIController;
	bool bIsPlayerVisible = false;
	UPROPERTY(EditAnywhere, Category = "AI")
	float DetectionRadius = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerKey;
};
