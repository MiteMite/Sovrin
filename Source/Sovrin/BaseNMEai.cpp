#include "BaseNMEai.h"

#include "ChasePlayerTask.h"
#include "NetworkMessage.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BehaviorTree/BTDecorator.h"
#include "Sovrin/BaseNME.h"
#include "Sovrin/Saoirse.h"

ABaseNMEai::ABaseNMEai()
{
	// Create perception components
	NMEPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*NMEPerceptionComponent);
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 600.0f;
	SightSenseConfig->LoseSightRadius = 700.0f;
	SightSenseConfig->PeripheralVisionAngleDegrees = 70.0f;
	SightSenseConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightSenseConfig->SetMaxAge(100.0f);
	NMEPerceptionComponent->ConfigureSense(*SightSenseConfig);
	NMEPerceptionComponent->SetDominantSense(SightSenseConfig->GetSenseImplementation());
	
	// Create and initialize the custom Behavior Tree
	BehaviorTree = CreateDefaultSubobject<UBehaviorTree>(TEXT("BehaviorTree"));
	BlackboardData = CreateDefaultSubobject<UBlackboardData>(TEXT("BlackboardData"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	
	//Setup Blackboard key values
	PlayerLocation.EntryName = FName("PlayerLocation");
	PlayerLocation.KeyType = PlayerLocationKeyType;
	BlackboardData->Keys.Add(PlayerLocation);
	PatrolPointLocation.EntryName = FName("PatrolPointLocation");
	PatrolPointLocation.KeyType = PatrolPointLocationKeyType;
	BlackboardData->Keys.Add(PatrolPointLocation);
	IsPlayerVisible.EntryName = FName("IsPlayerVisible");
	IsPlayerVisible.KeyType = IsPlayerVisibleKeyType;
	BlackboardData->Keys.Add(IsPlayerVisible);
	IsPlayerNOTVisible.EntryName = FName("IsPlayerNOTVisible");
	IsPlayerNOTVisible.KeyType = IsPlayerNOTVisibleKeyType;
	BlackboardData->Keys.Add(IsPlayerNOTVisible);
	
	//Set current patrol point index to 0
	CurrentPatrolPointIndexINT32 = 0;
	//Set patrol points to empty array
	ControllerPatrolPoints.Empty();
	//Initialize Blackboard
	BlackboardComponent->InitializeBlackboard(*BlackboardData);
	/*******************************************************************************************************************
	 *
	 *Begin Setup of node sequencer 
	 *
	 ******************************************************************************************************************/
	if (BehaviorTree && BehaviorTreeComponent)
		// Root sequence setup
	    RootSequence = NewObject<UBTComposite_Sequence>(BehaviorTreeComponent);
		RootSequence->NodeName = "Root";
		RootSequence->SetOwner(this->GetOwner());
	    BehaviorTree->RootNode = RootSequence;
		
	    // Main selector to differentiate between chase and patrol
	    MainSelector = NewObject<UBTComposite_Selector>(RootSequence);
		MainSelector->NodeName = "MainSelector";
		MainSelector->SetOwner(this->GetOwner());
	    FBTCompositeChild MainSelectorChild;
	    MainSelectorChild.ChildComposite = MainSelector;
	    RootSequence->Children.Add(MainSelectorChild);

		// *** CHASE SEQUENCE ***
		ChaseSequence = NewObject<UBTComposite_Sequence>(MainSelector);
		ChaseSequence->NodeName = "ChaseSequence";
		FBTCompositeChild ChaseSequenceChild;
		ChaseSequenceChild.ChildComposite = ChaseSequence;
		MainSelector->Children.Add(ChaseSequenceChild);
					
		// Chase Player Task inside ChaseSequence
		ChasePlayerTask = NewObject<UChasePlayerTask>(ChaseSequence);
		ChasePlayerTask->NodeName = "ChasePlayerTask";
		ChasePlayerTask->SetOwner(this->GetOwner());
		ChasePlayerTask->PlayerLocationKey.SelectedKeyName = PlayerLocation.EntryName; // PlayerLocation Blackboard Key
		FBTCompositeChild ChasePlayerTaskChild;
		ChasePlayerTaskChild.ChildTask = ChasePlayerTask;
		ChaseSequence->Children.Add(ChasePlayerTaskChild);
		
		// *** PATROL SEQUENCE ***
		PatrolSequence = NewObject<UBTComposite_Sequence>(MainSelector);
		PatrolSequence->NodeName = "PatrolSequence";
		FBTCompositeChild PatrolSequenceChild;
		PatrolSequenceChild.ChildComposite = PatrolSequence;
		MainSelector->Children.Add(PatrolSequenceChild);
		
		// Patrol Task (e.g., finding patrol points)
		FindPatrolPointTask = NewObject<UFindPatrolPointTask>(PatrolSequence);
		FindPatrolPointTask->NodeName = "FindPatrolPointTask";
		FindPatrolPointTask->SetOwner(this->GetOwner());
		FindPatrolPointTask->GetTargetLocationKey().SelectedKeyName = PatrolPointLocation.EntryName; // PatrolPointLocation Blackboard Key
		FBTCompositeChild FindPatrolTaskChild;
		FindPatrolTaskChild.ChildTask = FindPatrolPointTask;
		PatrolSequence->Children.Add(FindPatrolTaskChild);

		// MoveToPatrolPointTask after finding patrol points
		MoveToPatrolPointTask = NewObject<UMoveToPatrolPointTask>(PatrolSequence);
		MoveToPatrolPointTask->NodeName = "MoveToPatrolPointTask";
		MoveToPatrolPointTask->SetOwner(this->GetOwner());
		MoveToPatrolPointTask->GetTargetLocationKey().SelectedKeyName = PatrolPointLocation.EntryName; // PatrolPointLocation Blackboard Key
		FBTCompositeChild MoveToPatrolPointTaskChild;
		MoveToPatrolPointTaskChild.ChildTask = MoveToPatrolPointTask;
		PatrolSequence->Children.Add(MoveToPatrolPointTaskChild);

		// WaitTask after moving to patrol point
		WaitTask = NewObject<UAbortableWaitTask>(PatrolSequence);
		WaitTask->NodeName = "WaitTask";
		WaitTask->SetOwner(this->GetOwner());
		FBTCompositeChild WaitTaskChild;
		WaitTaskChild.ChildTask = WaitTask;
		PatrolSequence->Children.Add(WaitTaskChild);
	
		// *** Decorators and Services ***

		IsPlayerVisibleDecorator = NewObject<UIsPlayerVisibleDecorator>(ChaseSequence);
		IsPlayerVisibleDecorator->NodeName = "IsPlayerVisible";
		IsPlayerVisibleDecorator->SetOwner(this->GetOwner());
		ChaseSequenceChild.Decorators.Add(IsPlayerVisibleDecorator);
	
		/*IsPlayerNOTVisibleDecorator = NewObject<UIsPlayerNOTVisibleDecorator>(PatrolSequence);
		IsPlayerNOTVisibleDecorator->NodeName = "IsPlayerNOTVisible";
		IsPlayerNOTVisibleDecorator->SetOwner(this->GetOwner());
		PatrolSequenceChild.Decorators.Add(IsPlayerNOTVisibleDecorator);*/
	
		// DetectPlayerService is added as a service to the RootSequence
		DetectService = NewObject<UDetectPlayerService>(RootSequence);
		DetectService->NodeName = "DetectPlayerService";
		DetectService->SetOwner(this->GetOwner());
		RootSequence->Services.Add(DetectService);
}
	/*******************************************************************************************************************
	*
	*End Setup of node sequencer 
	*
	*******************************************************************************************************************/
	
void ABaseNMEai::BeginPlay()
{
	Super::BeginPlay();
	// Get patrol points from the controlled pawn
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controlled Pawn is null"));
		return;
	}
	ABaseNME* ParentBaseNME = Cast<ABaseNME>(ControlledPawn);
	if (!ParentBaseNME)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controlled Pawn is not a BaseNME"));
		return;
	}
	if (ParentBaseNME->IsA(ABaseNME::StaticClass()))
	{
		SetControllerPatrolPoints(ParentBaseNME->PatrolPoints);
		BlackboardComponent->SetValueAsVector("PatrolPointLocation", GetCurrentPatrolPoint()->GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning actor is not a BaseNME"));
	}

	
	if (BehaviorTree && BlackboardComponent)
	{
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
	return Cast<UBehaviorTreeComponent>(BrainComponent); // BrainComponent is inherited from AAIController
}

void ABaseNMEai::LogActiveBehaviorTreeNode()
{
	UBehaviorTreeComponent* BTComponent = ABaseNMEai::GetBehaviorTreeComponent();
	if (!BTComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load behavior tree component"));
		return;
	}

	ActiveNode = BTComponent->GetActiveNode();
	if (ActiveNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Active Node is: %s"), *ActiveNode->GetNodeName());
	}
}

void ABaseNMEai::LogBlackboardKeys()
{
	for (FBlackboardEntry& Key : BlackboardData->Keys)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blackboard Key Name: %s"), *Key.EntryName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Blackboard Key Value: %s"), *BlackboardComponent->GetValueAsVector(Key.EntryName).ToString());
	}
	
}

void ABaseNMEai::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/*if (BlackboardComponent)
	{
		bool bPlayerVisible = BlackboardComponent->GetValueAsBool("IsPlayerVisible");
		UE_LOG(LogTemp, Warning, TEXT("Current State - PlayerVisible: %s"), 
			   bPlayerVisible ? TEXT("TRUE") : TEXT("FALSE"));
	}*/
	LogActiveBehaviorTreeNode();
	//LogBlackboardKeys();
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
		for (AActor* Actor : ControllerPatrolPoints)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Patrol Point Location: %s"), *Actor->GetActorLocation().ToString());
		}
		CurrentPatrolPointIndexINT32++;
		CurrentPatrolPointIndexINT32 = CurrentPatrolPointIndexINT32 % ControllerPatrolPoints.Num();//increment patrol point index
		//UE_LOG(LogTemp, Warning, TEXT("Current Patrol Point Index: %d"), CurrentPatrolPointIndexINT32);
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

