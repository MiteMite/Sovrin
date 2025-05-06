#pragma once
#include "AIController.h"
#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	
	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	AActor* GetNextPatrolPoint();
	
	UFUNCTION(BlueprintCallable, Category="AI|Patrol")
	AActor* GetCurrentPatrolPoints(){ return nullptr;}

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
	int32 CurrentPatrolPointIndexINT32 = 0;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTree;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBlackboardData* BlackboardData;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> PatrolPoints;

	int32 PatrolPointIndex;
	
	virtual ~ABaseNMEai() override;
};
