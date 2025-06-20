// 

#pragma once

#include "CoreMinimal.h"
#include "Pickupable.generated.h"


UINTERFACE(MinimalAPI)
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

class SOVRIN_API IPickupable : public IInterface
{
	GENERATED_BODY()

public:
	virtual bool Pickup();
	
};
