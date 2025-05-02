#include "BaseNMEai.h"

#include "Sovrin/BaseNME.h"
#include "Sovrin/Saoirse.h"

ABaseNMEai::ABaseNMEai()
{
	NMEPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*NMEPerceptionComponent);
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 400.0f;
	SightSenseConfig->LoseSightRadius = 700.0f;
	SightSenseConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightSenseConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightSenseConfig->SetMaxAge(60.0f);
	NMEPerceptionComponent->ConfigureSense(*SightSenseConfig);
	NMEPerceptionComponent->SetDominantSense(SightSenseConfig->GetSenseImplementation());
	NMEPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseNMEai::OnTargetSighted);
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

ABaseNMEai::~ABaseNMEai()
{
	
}

