#include "BaseNME.h"

#include "BaseNMEai.h"

ABaseNME::ABaseNME()
{
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> UAnimationClass(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> USkeletalMeshObject(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	this->GetMesh()->SetSkeletalMesh(USkeletalMeshObject.Object);
	this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	AIControllerClass = ABaseNMEai::StaticClass();
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
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABaseNME::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/*UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();
	AnimInstance->AddCurveValue(TEXT("GroundSpeed"),this->GetVelocity().Size());
	UE_LOG(LogTemp, Display, TEXT("My Current speed is %f"),this->GetVelocity().Size());
	*/
}

ABaseNME::~ABaseNME()
{
	
}

