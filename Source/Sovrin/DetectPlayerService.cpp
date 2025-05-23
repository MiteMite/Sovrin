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
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller is null"));
		return;
	} 
	if (AIController->GetBlackboardComponent() && AIController->GetPerceptionComponent())
	{
		
		TArray<AActor*> DetectedActors;
		AIController->GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), DetectedActors);
		for (AActor* Actor : DetectedActors)
		{
			if (Actor->IsA(ASaoirse::StaticClass()))
			{
				AIController->GetBlackboardComponent()->SetValueAsVector("PlayerLocation", Actor->GetActorLocation());
				//AIController->GetBlackboardComponent()->GetValueAsVector("PlayerLocation").Set(Actor->GetActorLocation().X, Actor->GetActorLocation().Y, Actor->GetActorLocation().Z);
				//UE_LOG(LogTemp, Display, TEXT("Player location value on blackboard: %s"), *OwnerComp.GetBlackboardComponent()->GetValueAsVector("PlayerLocation").ToString());
				//UE_LOG(LogTemp, Display, TEXT("Player detected at location: %s"), *Actor->GetActorLocation().ToString());
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Blackboard or Perception component is null"));
	}
}
