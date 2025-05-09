#pragma once
#include "GameFramework/Character.h"
#include "TimeTravel.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseNMEai.h"
#include "animation/AnimInstance.h"
#include "animation/AnimBlueprint.h"
#include "animation/AnimBlueprintGeneratedClass.h"
#include "BaseNME.generated.h"

UCLASS()

class SOVRIN_API ABaseNME : public ACharacter
{
	GENERATED_BODY()
public:
	ABaseNME();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTimeTravel* TimeTravelComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol Points")
	TArray<AActor*> PatrolPoints;

private:
	virtual void Tick(float DeltaSeconds) override;
	virtual ~ABaseNME() override;
};
