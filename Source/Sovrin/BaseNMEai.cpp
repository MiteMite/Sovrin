#include "BaseNMEai.h"

#include "BaseBehaviorTree.h"
#include "Sovrin/BaseNME.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
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
	if (BlackboardData && BehaviorTree)
	{
		UseBlackboard(BehaviorTree->GetBlackboardAsset(), BlackboardComponent);
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

