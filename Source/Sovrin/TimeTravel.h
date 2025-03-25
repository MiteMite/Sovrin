#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeTravel.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API ATimeTravel : public AActor
{
	GENERATED_BODY()
public:
	ATimeTravel();
	
protected:
	virtual void BeginPlay() override;

	virtual ~ATimeTravel() override;
};
