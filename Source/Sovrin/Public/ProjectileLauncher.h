// 

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileLauncher.generated.h"

UCLASS()
class SOVRIN_API AProjectileLauncher : public AWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileLauncher();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float LaunchSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
