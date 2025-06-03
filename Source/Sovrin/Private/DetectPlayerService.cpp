// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectPlayerService.h"
#include "behaviortree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseNMEai.h"
#include "Saoirse.h"
#include "Kismet/GameplayStatics.h"

UDetectPlayerService::UDetectPlayerService()
{
	NodeName = TEXT("Detect Player Service");
	Interval = 0.5f;
	AIController = nullptr;
	//UE_LOG(LogTemp, Warning, TEXT("My owner is %s"),*this->GetFullName());
}

void UDetectPlayerService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
	AIController = OwnerComp.GetAIOwner();
    
	if (!AIController || !AIController->GetBlackboardComponent() || !AIController->GetPerceptionComponent())
	{
		return;
	}
    
	TArray<AActor*> DetectedActors;
	AIController->GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), DetectedActors);

	bool bPreviousVisibility = bIsPlayerVisible;
	bIsPlayerVisible = false;
	FVector PlayerLocation = FVector::ZeroVector;
    
	// Check if player is detected
	for (AActor* Actor : DetectedActors)
	{
		if (Actor && Actor->IsA(ASaoirse::StaticClass()))
		{
			bIsPlayerVisible = true;
			PlayerLocation = Actor->GetActorLocation();
			break;
		}
	}
    
	// Always update blackboard
	AIController->GetBlackboardComponent()->SetValueAsBool("IsPlayerVisible", bIsPlayerVisible);
    
	if (bIsPlayerVisible)
	{
		AIController->GetBlackboardComponent()->SetValueAsVector("PlayerLocation", PlayerLocation);
	}
    
	// Debug log
	if (bPreviousVisibility != bIsPlayerVisible)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Player visibility changed to: %s"),
			   bIsPlayerVisible ? TEXT("VISIBLE") : TEXT("NOT VISIBLE"));*/
	}
}
