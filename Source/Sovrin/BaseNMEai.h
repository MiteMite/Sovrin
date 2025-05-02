#pragma once
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BaseNMEai.generated.h"

UCLASS()
class SOVRIN_API ABaseNMEai : public AAIController
{
	GENERATED_BODY()
public:
	ABaseNMEai();
	
	UFUNCTION()
	void OnTargetSighted(const TArray<AActor*>& Targets);
	
private:
	UPROPERTY(EditAnywhere, Category = "Perception")
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
