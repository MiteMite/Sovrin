#include "BaseNMEai.h"

#include "BaseNME.h"
#include "Saoirse.h"

ABaseNMEai::ABaseNMEai()
{
	NMEPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*NMEPerceptionComponent);
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 600.0f;
	SightSenseConfig->LoseSightRadius = 1000.0f;
	SightSenseConfig->PeripheralVisionAngleDegrees = 180.0f;
	SightSenseConfig->SetMaxAge(15.0f);
	NMEPerceptionComponent->ConfigureSense(*SightSenseConfig);
	NMEPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseNMEai::OnTargetSighted);
	NMEPawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	NMEPawnSensingComponent->SightRadius = 900.0f;
	NMEPawnSensingComponent->SetPeripheralVisionAngle(180.0f);
	NMEPawnSensingComponent->OnSeePawn.AddDynamic(this, &ABaseNMEai::OnPawnSighted);
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

void ABaseNMEai::OnPawnSighted(APawn* SpottedPawn)
{
	UE_LOG(LogTemp, Display, TEXT("Target sighted"));
	MoveToLocation(SpottedPawn->GetActorLocation());
}

ABaseNMEai::~ABaseNMEai()
{
	
}

