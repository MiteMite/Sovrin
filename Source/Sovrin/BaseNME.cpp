#include "BaseNME.h"

#include "BaseNMEai.h"

ABaseNME::ABaseNME()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	this->GetMesh()->SetSkeletalMesh(USkeletalMeshObject.Object);
	this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	AIControllerClass = ABaseNMEai::StaticClass();
	TimeTravelComponent = CreateDefaultSubobject<UTimeTravel>(TEXT("TimeTravel"));
	this->GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

ABaseNME::~ABaseNME()
{
	
}

