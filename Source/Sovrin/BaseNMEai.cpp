#include "BaseNMEai.h"

#include "BaseNME.h"
#include "Saoirse.h"

ABaseNMEai::ABaseNMEai()
{
	NMEPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*NMEPerceptionComponent);
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 300.0f;
	SightSenseConfig->LoseSightRadius = 1000.0f;
	SightSenseConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightSenseConfig->SetMaxAge(15.0f);
	NMEPerceptionComponent->ConfigureSense(*SightSenseConfig);
	NMEPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseNMEai::OnTargetSighted);
}

void ABaseNMEai::OnTargetSighted(const TArray<AActor*>& Targets)
{
	UE_LOG(LogTemp, Display, TEXT("Target sighted"));
	for (AActor* Target : Targets)
	{
		if (Target->IsA(ASaoirse::StaticClass()))
		{
			MoveToLocation(Target->GetActorLocation());
			UE_LOG(LogTemp, Display, TEXT("Target sighted"));
		}
	}
}

ABaseNMEai::~ABaseNMEai()
{
	
}

