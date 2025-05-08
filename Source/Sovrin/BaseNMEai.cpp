#include "BaseNMEai.h"
#include "Sovrin/BaseNME.h"
#include "Sovrin/Saoirse.h"

ABaseNMEai::ABaseNMEai()
{
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
	NMEPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseNMEai::OnTargetSighted);
	BehaviorTree = CreateDefaultSubobject<UBehaviorTree>(TEXT("BehaviorTree"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BlackboardData = CreateDefaultSubobject<UBlackboardData>(TEXT("BlackboardData"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeObject(TEXT("/Game/SovrinClasses/AiAssets/BT_BaseNME.BT_BaseNME"));
	ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardDataObject(TEXT("/Game/SovrinClasses/AiAssets/BB_BaseNME.BB_BaseNME"));
	BehaviorTree = BehaviorTreeObject.Object;
	BlackboardData = BlackboardDataObject.Object;
	
}

void ABaseNMEai::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree && BlackboardData)
	{
		UseBlackboard(BlackboardData,BlackboardComponent);
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

AActor* ABaseNMEai::GetNextPatrolPoint()
{
	if (PatrolPoints.Num()>0)
	{
		return nullptr;
	}
	CurrentPatrolPointIndexINT32 = (CurrentPatrolPointIndexINT32+1)%PatrolPoints.Num();
	return PatrolPoints[CurrentPatrolPointIndexINT32];
}

AActor* ABaseNMEai::GetCurrentPatrolPoints()
{
	return PatrolPoints[CurrentPatrolPointIndexINT32];
}

ABaseNMEai::~ABaseNMEai()
{
	
}

