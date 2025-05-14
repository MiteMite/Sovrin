// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectPlayerService.h"
#include "behaviortree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UDetectPlayerService::UDetectPlayerService()
{
	NodeName = TEXT("Detect Player");
	Interval = 0.5f;
	//UE_LOG(LogTemp, Warning, TEXT("My owner is %s"),*this->GetFullName());
}

void UDetectPlayerService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!AIController && !BlackboardComponent)
		return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
		return;

	float Distance = (PlayerPawn->GetActorLocation() - AIController->GetPawn()->GetActorLocation()).Size();
	if (Distance < DetectionRadius)
	{
		BlackboardComponent->SetValueAsObject(PlayerKey.SelectedKeyName, PlayerPawn);
	}
	else
	{
		BlackboardComponent->ClearValue(PlayerKey.SelectedKeyName);
	}
}
