// Fill out your copyright notice in the Description page of Project Settings.


#include "Sovrin/Public/BaseBehaviorTree.h"

#include "Sovrin/FindPatrolPointTask.h"

UBaseBehaviorTree::UBaseBehaviorTree()
{
	//UE_LOG(LogTemp, Warning, TEXT("BaseBehaviorTree created"));
	//create and setup blackboard
	BaseBlackboardData = CreateDefaultSubobject<UBlackboardData>(TEXT("BaseBlackboardData"));
	//add blackboard keys for player location
	if (BaseBlackboardData)
	{
		//UE_LOG(LogTemp, Warning, TEXT("BaseBlackboardData created"));
		FBlackboardEntry PlayerLocation = *new FBlackboardEntry();
		UBlackboardKeyType_Vector* PlayerLocationKeyType = NewObject<UBlackboardKeyType_Vector>();
		PlayerLocation.EntryName = FName("PlayerLocation");
		PlayerLocation.KeyType = PlayerLocationKeyType;
		BaseBlackboardData->Keys.Add(PlayerLocation); //add key to blackboard
		//for patrol point locations
		FBlackboardEntry PatrolPointLocation =  *new FBlackboardEntry();
		UBlackboardKeyType_Vector* PatrolPointLocationKeyType = NewObject<UBlackboardKeyType_Vector>();
		PatrolPointLocation.EntryName = FName("PatrolPointLocation");
		PatrolPointLocation.KeyType = PatrolPointLocationKeyType;
		BaseBlackboardData->Keys.Add(PatrolPointLocation); //add key to blackboard
	}
	//create behavior tree
	BaseBehaviorTreeComponent = NewObject<UBehaviorTreeComponent>();
	BaseBehaviorTree = NewObject<UBehaviorTree>();
	if (BaseBehaviorTree && BaseBehaviorTreeComponent)
	{
		// Create root Sequence node
		UBTComposite_Sequence* RootSequence = NewObject<UBTComposite_Sequence>(BaseBehaviorTreeComponent);
		BaseBehaviorTree->RootNode = RootSequence;
		
		// Add DetectPlayerService as a service to the root
		UDetectPlayerService* DetectService = NewObject<UDetectPlayerService>(RootSequence);
		RootSequence->Services.Add(DetectService);
		//UE_LOG(LogTemp, Warning, TEXT("Detect player service created %s"),*RootSequence->Services.GetData()->GetFullName());
		
		// Create main Selector node and its child connection
		UBTComposite_Selector* MainSelector = NewObject<UBTComposite_Selector>(RootSequence);
		FBTCompositeChild MainSelectorChild;
		MainSelectorChild.ChildComposite = MainSelector;
		RootSequence->Children.Add(MainSelectorChild);

		//create patrol point selector
		UBTComposite_Selector* PatrolSelector = NewObject<UBTComposite_Selector>(MainSelector);
		FBTCompositeChild PatrolSelectorChild;
		PatrolSelectorChild.ChildComposite = PatrolSelector;
		MainSelector->Children.Add(PatrolSelectorChild);
		//create patrol point task
		UFindPatrolPointTask* PatrolPointTask = NewObject<UFindPatrolPointTask>();
		PatrolSelectorChild.ChildTask = PatrolPointTask;
    
		// Create Chase Player sequence
		UBTComposite_Sequence* ChaseSequence = NewObject<UBTComposite_Sequence>(MainSelector);
		FBTCompositeChild ChaseSequenceChild;
		ChaseSequenceChild.ChildComposite = ChaseSequence;
		MainSelector->Children.Add(ChaseSequenceChild);
		//UE_LOG(LogTemp, Warning, TEXT("Root has this many children: %i"), BaseBehaviorTree->RootNode->GetChildrenNum());
	}
}

UBehaviorTreeComponent* UBaseBehaviorTree::GetBehaviorTreeComponent()
{
	return BaseBehaviorTreeComponent;
}

UBlackboardComponent* UBaseBehaviorTree::GetBlackboardComponent()
{
	return BaseBlackboardComponent;
}
UBlackboardData* UBaseBehaviorTree::GetBlackboardData()
{
	return BaseBlackboardData;
}
UBehaviorTree* UBaseBehaviorTree::GetBehaviorTree()
{
	return BaseBehaviorTree;
}
void UBaseBehaviorTree::SetBaseBlackboardComponent(UBlackboardComponent* BlackboardComponent)
{
	BaseBlackboardComponent = BlackboardComponent;
}
