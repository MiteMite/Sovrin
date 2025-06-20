﻿#pragma once
#include "AIController.h"
#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BehaviorTree/BTDecorator.h"
#include "IsPlayerVisibleDecorator.h"
#include "IsPlayerNOTVisibleDecorator.h"
#include "BehaviorTree/Composites/BTComposite_SimpleParallel.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "AbortableWaitTask.h"
#include "DetectPlayerService.h"
#include "FindPatrolPointTask.h"
#include "MoveToPatrolPointTask.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "IsPlayerVisibleDecorator.h"
#include "AITask_MoveTo.generated.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ChasePlayerTask.h"
#include "BaseNMEai.generated.h"

UCLASS()
class SOVRIN_API ABaseNMEai : public AAIController
{
	GENERATED_BODY()
public:
	ABaseNMEai();
	
	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	TArray<AActor*> GetControllerPatrolPoints();
	
	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	AActor* GetNextPatrolPoint();
	
	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	AActor* GetCurrentPatrolPoint();

	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	void SetControllerPatrolPoints(TArray<AActor*> PatrolPoints);

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
private:
	
	UBehaviorTreeComponent* GetBehaviorTreeComponent();
	void LogActiveBehaviorTreeNode();
	void LogBlackboardKeys();

	UPROPERTY()
	const UBTNode* ActiveNode;
	
	UPROPERTY()
	TArray<UBTTaskNode*> ActiveTasks;
	
	//blackboard keys
	FBlackboardEntry PlayerLocation = *new FBlackboardEntry();
	
	UPROPERTY()
	UBlackboardKeyType_Vector* PlayerLocationKeyType = NewObject<UBlackboardKeyType_Vector>();

	UPROPERTY()
	FBlackboardEntry PatrolPointLocation =  *new FBlackboardEntry();

	UPROPERTY()
	UBlackboardKeyType_Vector* PatrolPointLocationKeyType = NewObject<UBlackboardKeyType_Vector>();
	FBlackboardEntry IsPlayerVisible = *new FBlackboardEntry();

	UPROPERTY()
	UBlackboardKeyType_Bool* IsPlayerVisibleKeyType = NewObject<UBlackboardKeyType_Bool>();
	FBlackboardEntry IsPlayerNOTVisible = *new FBlackboardEntry();

	UPROPERTY()
	UBlackboardKeyType_Bool* IsPlayerNOTVisibleKeyType = NewObject<UBlackboardKeyType_Bool>();

	
	//Behavior tree nodes services and tasks

	UPROPERTY()
	UBTComposite_Sequence* RootSequence;
	
	UPROPERTY()
	UDetectPlayerService* DetectService;

	UPROPERTY()
	UBTComposite_Selector* MainSelector;

	UPROPERTY()
	UBTComposite_Sequence* ChaseSequence;

	UPROPERTY()
	UChasePlayerTask* ChasePlayerTask;

	UPROPERTY()
	UBTComposite_Sequence* PatrolSequence;

	UPROPERTY()
	UBTComposite_Sequence* PatrolPointTaskSequence;

	UPROPERTY()
	UFindPatrolPointTask* FindPatrolPointTask;

	UPROPERTY()
	UMoveToPatrolPointTask* MoveToPatrolPointTask;

	UPROPERTY()
	UIsPlayerVisibleDecorator* IsPlayerVisibleDecorator;

	UPROPERTY()
	UIsPlayerNOTVisibleDecorator* IsPlayerNOTVisibleDecorator;

	UPROPERTY()
	UAbortableWaitTask* WaitTask;
	
	//patrol points
	UPROPERTY(EditAnywhere, Category = "AI")
	TArray<AActor*> ControllerPatrolPoints;
	UPROPERTY(EditAnywhere, Category = "Perception")
	UAIPerceptionComponent* NMEPerceptionComponent;
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<UAISenseConfig> SightConfig;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightRadius;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightAngle;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightHearingThreshold;
	UPROPERTY(EditAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightSenseConfig;
	int32 CurrentPatrolPointIndexINT32 = 0;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTree;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBlackboardData* BlackboardData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	int32 PatrolPointIndex;
	
	virtual ~ABaseNMEai() override;
};
