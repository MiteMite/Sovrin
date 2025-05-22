#include "BaseNMEai.h"

#include "BaseBehaviorTree.h"
#include "ChasePlayerTask.h"
#include "Sovrin/BaseNME.h"
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
	
	// Create and initialize the custom Behavior Tree
	BehaviorTree = CreateDefaultSubobject<UBaseBehaviorTree>(TEXT("BehaviorTree"));
	BlackboardData = CreateDefaultSubobject<UBlackboardData>(TEXT("BlackboardData"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent->InitializeBlackboard(*BlackboardData);
	//Setup Blackboard key values
	PlayerLocation.EntryName = FName("PlayerLocation");
	PlayerLocation.KeyType = PlayerLocationKeyType;
	BlackboardData->Keys.Add(PlayerLocation);
	PatrolPointLocation.EntryName = FName("PatrolPointLocation");
	PatrolPointLocation.KeyType = PatrolPointLocationKeyType;
	BlackboardData->Keys.Add(PatrolPointLocation);
	//Set current patrol point index to 0
	CurrentPatrolPointIndexINT32 = 0;
	//Set patrol points to empty array
	ControllerPatrolPoints.Empty();

	/*******************************************************************************************************************
	 *
	 *Begin Setup of node sequencer 
	 *
	 ******************************************************************************************************************/
	if (BehaviorTree && BehaviorTreeComponent)
	{
		// Create root Sequence node
		RootSequence = NewObject<UBTComposite_Sequence>(BehaviorTreeComponent);
		RootSequence->NodeName = TEXT("Root");
		BehaviorTree->RootNode = RootSequence;
		
		// Add DetectPlayerService as a service to the root
		DetectService = NewObject<UDetectPlayerService>(RootSequence);
		DetectService->SetOwner(this->GetOwner());
		RootSequence->Services.Add(DetectService);
		
		//Chase Player sequence
		ChaseSequence = NewObject<UBTComposite_Sequence>(RootSequence);
		ChaseSequence->NodeName = TEXT("Chase Sequence");
		FBTCompositeChild ChaseSequenceChild;
		ChaseSequenceChild.ChildComposite = ChaseSequence;
		RootSequence->Children.Add(ChaseSequenceChild);
		
		//Chase Player Task
		UChasePlayerTask* ChasePlayerTask = NewObject<UChasePlayerTask>(ChaseSequence);
		ChasePlayerTask->SetOwner(this->GetOwner());
		ChasePlayerTask->NodeName = TEXT("Chase Player Task");
		ChaseSequenceChild.ChildTask = ChasePlayerTask;
		
		// Create main Selector node and its child connection
		MainSelector = NewObject<UBTComposite_Selector>(RootSequence);
		MainSelector->NodeName = TEXT("Main Selector");
		FBTCompositeChild MainSelectorChild;
		MainSelectorChild.ChildComposite = MainSelector;
		RootSequence->Children.Add(MainSelectorChild);

		//create patrol point selector
		PatrolSelector = NewObject<UBTComposite_Selector>(MainSelector);
		PatrolSelector->NodeName = TEXT("Patrol Selector");
		FBTCompositeChild PatrolSelectorChild;
		PatrolSelectorChild.ChildComposite = PatrolSelector;
		MainSelector->Children.Add(PatrolSelectorChild);
		
		//find patrol point task
		PatrolPointTask = NewObject<UFindPatrolPointTask>(PatrolSelector);
		PatrolPointTask->SetOwner(this->GetOwner());
		PatrolPointTask->NodeName = TEXT("Patrol Point Task");
		PatrolSelectorChild.ChildTask = PatrolPointTask;
		
		//Behavior Tree Composite for Patrol
		PatrolSequence = NewObject<UBTComposite_Sequence>(PatrolSelector);
		PatrolSequence->NodeName = TEXT("Patrol Sequence");
		FBTCompositeChild PatrolSequenceChild;
		PatrolSequenceChild.ChildComposite = PatrolSequence;
		PatrolSelector->Children.Add(PatrolSequenceChild);
		
	}
	/*******************************************************************************************************************
	*
	*End Setup of node sequencer 
	*
	*******************************************************************************************************************/
}
	
void ABaseNMEai::BeginPlay()
{
	Super::BeginPlay();
	// Get patrol points from the controlled pawn
	if (ABaseNME* BaseNME = Cast<ABaseNME>(GetPawn()))
	{
		SetControllerPatrolPoints(BaseNME->PatrolPoints);
	}
	
	if (BehaviorTree && BlackboardComponent)
	{
		//NMEPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseNMEai::OnTargetSighted);
		UseBlackboard(BlackboardData, BlackboardComponent);
		RunBehaviorTree(BehaviorTree);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load behavior tree or blackboard component"));
	}
	

}

UBehaviorTreeComponent* ABaseNMEai::GetBehaviorTreeComponent()
{
	
	return BehaviorTreeComponent; // BrainComponent is inherited from AAIController
}

void ABaseNMEai::LogActiveBehaviorTreeNode()
{
	UBehaviorTreeComponent* BTComponent = GetBehaviorTreeComponent();
	if (!BTComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load behavior tree component"));
		return;
	}

	ActiveNode = BTComponent->GetActiveNode();
	if (ActiveNode)
	{
		UE_LOG(LogTemp, Display, TEXT("Active Node is: %s"), *ActiveNode->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No active node currently"));
	}

	ActiveTask = Cast<UBTTaskNode>(ActiveNode);
	if (ActiveTask)
	{
		UE_LOG(LogTemp, Display, TEXT("Active Task is: %s"), *ActiveTask->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No active task currently"));
	}
}


void ABaseNMEai::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	LogActiveBehaviorTreeNode();
}

void ABaseNMEai::OnTargetSighted(const TArray<AActor*>& Targets)
{
	//UE_LOG(LogTemp, Display, TEXT("Handle Ai"));
	for (AActor* Target : Targets)
	{
		if (Target->IsA(ASaoirse::StaticClass()))
		{
			//UE_LOG(LogTemp, Display, TEXT("Player detected: %s"),*Target->GetName());
			if (BlackboardComponent)
			{
				BlackboardComponent->SetValueAsVector(TEXT("PlayerLocation"), Target->GetActorLocation());
			}
			//UE_LOG(LogTemp, Display, TEXT("Pursuing target!"));
			//MoveToLocation(Target->GetActorLocation());
		}
	}
}
void ABaseNMEai::OnPatrolPointTask()
{
	UE_LOG(LogTemp, Display, TEXT("Patrol point task"));
	if (ControllerPatrolPoints.Num()>0)
	{
		CurrentPatrolPointIndexINT32 = (CurrentPatrolPointIndexINT32 + 1) % ControllerPatrolPoints.Num();//increment patrol point index
		AActor* NextPatrolPoint = ControllerPatrolPoints[CurrentPatrolPointIndexINT32];

		if (BlackboardComponent && NextPatrolPoint)
		{
			UE_LOG(LogTemp, Display, TEXT("Patrol point location: %s"),*NextPatrolPoint->GetName());
			BlackboardComponent->SetValueAsVector(TEXT("PatrolPointLocation"), NextPatrolPoint->GetActorLocation());
		}
		if (NextPatrolPoint)
		{
			UE_LOG(LogTemp, Display, TEXT("Moving to patrol point!"));
			MoveToLocation(NextPatrolPoint->GetActorLocation());
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
		CurrentPatrolPointIndexINT32 = (CurrentPatrolPointIndexINT32 + 1) % ControllerPatrolPoints.Num();//increment patrol point index
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

