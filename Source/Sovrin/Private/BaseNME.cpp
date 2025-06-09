#include "BaseNME.h"

ABaseNME::ABaseNME()
{
	Tags.Add(FName("Enemy")); 
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> UAnimationClass(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	this->GetMesh()->SetSkeletalMesh(USkeletalMeshObject.Object);
	this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	TimeTravelComponent = CreateDefaultSubobject<UTimeTravel>(TEXT("TimeTravel"));
	this->GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	//bind animation blueprint
	if (UAnimationClass.Object!=nullptr)
	{
		this->GetMesh()->SetAnimInstanceClass(UAnimationClass.Object->GeneratedClass);
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Aint no animation blueprint"));
	}
	AIControllerClass = ABaseNMEai::StaticClass();
	PatrolPoints = AIControllerClass->GetDefaultObject<ABaseNMEai>()->GetControllerPatrolPoints();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABaseNME::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

ABaseNME::~ABaseNME()
{
	
}

