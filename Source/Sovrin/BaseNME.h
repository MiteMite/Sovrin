#pragma once
#include "GameFramework/Character.h"
#include "TimeTravel.h"
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
private:
	virtual void Tick(float DeltaSeconds) override;
	virtual ~ABaseNME() override;
};
