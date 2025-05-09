#include "BaseNMEai.h"

#include "Sovrin/BaseNME.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardAssetProvider.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Sovrin/Saoirse.h"

ABaseNMEai::ABaseNMEai()
{
	// Create perception components
	NMEPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*NMEPerceptionComponent);
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 600.0f;
	SightSenseConfig->LoseSightRadius = 100.0f;
	SightSenseConfig->PeripheralVisionAngleDegrees = 70.0f;
	SightSenseConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightSenseConfig->SetMaxAge(100.0f);
	NMEPerceptionComponent->ConfigureSense(*SightSenseConfig);
	NMEPerceptionComponent->SetDominantSense(SightSenseConfig->GetSenseImplementation());

	// Create AI components
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    // Create and setup the Blackboard
    BlackboardData = NewObject<UBlackboardData>();
    if (BlackboardData)
    {
		/*
    	// Add Blackboard keys
        UBlackboardDataFactory* BBFactory = NewObject<UBlackboardDataFactory>();
        
        // Add Vector key for target location
        FBlackboardEntry TargetLocationEntry;
        TargetLocationEntry.EntryName = FName("TargetLocation");
        TargetLocationEntry.KeyType = UBlackboardKeyType_Vector::StaticClass();
        BlackboardData->Keys.Add(TargetLocationEntry);
        
        // Add Boolean key for player visibility
        FBlackboardEntry IsPlayerVisibleEntry;
        IsPlayerVisibleEntry.EntryName = FName("IsPlayerVisible");
        IsPlayerVisibleEntry.KeyType = UBlackboardKeyType_Bool::StaticClass();
        BlackboardData->Keys.Add(IsPlayerVisibleEntry);
        
        // Add Object key for current patrol point
        FBlackboardEntry CurrentPatrolPointEntry;
        CurrentPatrolPointEntry.EntryName = FName("CurrentPatrolPoint");
        CurrentPatrolPointEntry.KeyType = UBlackboardKeyType_Object::StaticClass();
        BlackboardData->Keys.Add(CurrentPatrolPointEntry);*/
    }

    // Create and setup the Behavior Tree
    BehaviorTree = NewObject<UBehaviorTree>();
    if (BehaviorTree)
    {
        BehaviorTree->BlackboardAsset = BlackboardData;
        
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
        /*
        // Add IsPlayerVisible decorator to chase sequence
        UIsPlayerVisibleDecorator* VisibleDecorator = NewObject<UIsPlayerVisibleDecorator>(ChaseSequence);
        ChaseSequence->Decorators.Add(VisibleDecorator);
        
        // Add MoveTo task for chasing player
        UBTTask_MoveTo* ChaseTask = NewObject<UBTTask_MoveTo>(ChaseSequence);
        ChaseTask->BlackboardKey.SelectedKeyName = "TargetLocation";
        ChaseSequence->Children.Add(ChaseTask);
        
        // Create Patrol sequence
        UBTComposite_Sequence* PatrolSequence = NewObject<UBTComposite_Sequence>(MainSelector);
        MainSelector->Children.Add(PatrolSequence);
        
        // Add FindPatrolPoint task
        UFindPatrolPointTask* FindPatrolTask = NewObject<UFindPatrolPointTask>(PatrolSequence);
        PatrolSequence->Children.Add(FindPatrolTask);
        
        // Add MoveTo task for patrol movement
        UBTTask_MoveTo* PatrolMoveTask = NewObject<UBTTask_MoveTo>(PatrolSequence);
        PatrolMoveTask->BlackboardKey.SelectedKeyName = "TargetLocation";
        PatrolSequence->Children.Add(PatrolMoveTask);*/
    }
}


void ABaseNMEai::BeginPlay()
{
	Super::BeginPlay();
	// Get patrol points from the controlled pawn
	if (ABaseNME* BaseNME = Cast<ABaseNME>(GetPawn()))
	{
		SetControllerPatrolPoints(BaseNME->PatrolPoints);
	}
	if (BlackboardData && BehaviorTree)
	{
		UseBlackboard(BlackboardData, BlackboardComponent);
		RunBehaviorTree(BehaviorTree);
	}
}
void ABaseNMEai::OnTargetSighted(const TArray<AActor*>& Targets)
{
	UE_LOG(LogTemp, Display, TEXT("Handle Ai"));
	for (AActor* Target : Targets)
	{
		UE_LOG(LogTemp, Display, TEXT("Target's name is: %s"),*Target->GetName());
		if (Target->IsA(ASaoirse::StaticClass()))
		{
			MoveToLocation(Target->GetActorLocation());
			UE_LOG(LogTemp, Display, TEXT("Pursuing target!"));
		}
	}
}

void ABaseNMEai::SetControllerPatrolPoints(TArray<AActor*> PatrolPoints)
{
	ControllerPatrolPoints = PatrolPoints;
}

TArray<AActor*> ABaseNMEai::GetControllerPatrolPoints()
{
	return ControllerPatrolPoints;
}

AActor* ABaseNMEai::GetNextPatrolPoint()
{
	if (ControllerPatrolPoints.Num()>0)
	{
		CurrentPatrolPointIndexINT32++;
		return ControllerPatrolPoints[CurrentPatrolPointIndexINT32];
	}
	else
	{
		return nullptr;
	}

}

AActor* ABaseNMEai::GetCurrentPatrolPoint()
{
	return ControllerPatrolPoints[CurrentPatrolPointIndexINT32];
}

ABaseNMEai::~ABaseNMEai()
{
	
}

