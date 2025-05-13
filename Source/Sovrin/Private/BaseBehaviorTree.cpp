// Fill out your copyright notice in the Description page of Project Settings.


#include "Sovrin/Public/BaseBehaviorTree.h"

UBaseBehaviorTree::UBaseBehaviorTree()
{
	//create and setup blackboard
	BlackboardAsset = CreateDefaultSubobject<UBlackboardData>(TEXT("Blackboard"));
	BlackboardAsset = BlackboardData;

	//add blackboard keys
	//for player location
	FBlackboardEntry* PlayerLocation = new FBlackboardEntry();
	UBlackboardKeyType_Vector* PlayerLocationKeyType = NewObject<UBlackboardKeyType_Vector>();
	PlayerLocation->EntryName = FName("PlayerLocation");
	PlayerLocation->KeyType = PlayerLocationKeyType;
	BlackboardData->Keys.Add(*PlayerLocation); //add key to blackboard
	//for patrol point locations
	FBlackboardEntry* PatrolPointLocation = new FBlackboardEntry();
	UBlackboardKeyType_Vector* PatrolPointLocationKeyType = NewObject<UBlackboardKeyType_Vector>();
	PatrolPointLocation->EntryName = FName("PatrolPointLocation");
	PatrolPointLocation->KeyType = PatrolPointLocationKeyType;
	BlackboardData->Keys.Add(*PatrolPointLocation); //add key to blackboard

	// Create and setup the custom Behavior Tree
    BehaviorTree = NewObject<UBaseBehaviorTree>();
    if (BehaviorTree)
    {
        // Create root Sequence node
        UBTComposite_Sequence* RootSequence = NewObject<UBTComposite_Sequence>(BehaviorTree);
        BehaviorTree->RootNode = RootSequence;
        
        // Add DetectPlayerService as a service to the root
        UDetectPlayerService* DetectService = NewObject<UDetectPlayerService>(RootSequence);
        RootSequence->Services.Add(DetectService);
        
    	// Create main Selector node and its child connection
    	UBTComposite_Selector* MainSelector = NewObject<UBTComposite_Selector>(RootSequence);
    	FBTCompositeChild MainSelectorChild;
    	MainSelectorChild.ChildComposite = MainSelector;
    	RootSequence->Children.Add(MainSelectorChild);
    	
        // Create Chase Player sequence
        UBTComposite_Sequence* ChaseSequence = NewObject<UBTComposite_Sequence>(MainSelector);
    	FBTCompositeChild ChaseSequenceChild;
    	ChaseSequenceChild.ChildComposite = ChaseSequence;
        MainSelector->Children.Add(ChaseSequenceChild);
    }
}