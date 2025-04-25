#pragma once
#include "GameFramework/Character.h"
#include "TimeTravel.h"
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

	virtual ~ABaseNME() override;
};
