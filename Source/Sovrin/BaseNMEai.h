#pragma once
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "BaseNMEai.generated.h"

UCLASS()
class SOVRIN_API ABaseNMEai : public AAIController
{
	GENERATED_BODY()
public:
	ABaseNMEai();
	
	UFUNCTION()
	void OnTargetSighted(const TArray<AActor*>& Targets);
	UFUNCTION()
	void OnPawnSighted(APawn* SpottedPawn);
	
private:
	UPROPERTY(EditAnywhere, Category = "Components")
	UPawnSensingComponent* NMEPawnSensingComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	UAIPerceptionComponent* NMEPerceptionComponent;
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<UAISenseConfig> SightConfig;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightRadius;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightAngle;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightHearingThreshold;
	UPROPERTY(EditAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightSenseConfig;
	
	virtual ~ABaseNMEai() override;
};
