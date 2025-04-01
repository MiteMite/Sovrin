#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimeTravel.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API UTimeTravel : public UActorComponent
{
	GENERATED_BODY()
public:
	UTimeTravel();
	
protected:
	virtual void BeginPlay() override;

	virtual ~UTimeTravel() override;
};
