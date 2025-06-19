// 


#include "Pickupable.h"


// Sets default values
APickupable::APickupable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APickupable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

