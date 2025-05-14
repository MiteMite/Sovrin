#include "BaseNMEai.h"

#include "BaseBehaviorTree.h"
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
    BehaviorTree = NewObject<UBaseBehaviorTree>();
	
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
		NMEPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseNMEai::OnTargetSighted);
	}
	
	if (BehaviorTree)
	{
		UseBlackboard(BehaviorTree->GetBlackboardData(), BlackboardComponent);
		BehaviorTree->SetBaseBlackboardComponent(BlackboardComponent);
		RunBehaviorTree(BehaviorTree);
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTree and blackboard running"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load behavior tree"));
	}
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
			MoveToLocation(Target->GetActorLocation());
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

