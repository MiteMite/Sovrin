// 


#include "Sovrin/Public/ProjectileLauncher.h"


// Sets default values
AProjectileLauncher::AProjectileLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProjectileLauncher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

