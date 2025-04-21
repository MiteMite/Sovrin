// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravel.h"

#include "ComponentUtils.h"
#include "MaterialHLSLTree.h"

UTimeTravel::UTimeTravel()
{
	//PrimaryComponentTick.bCanEverTick=true;
	
}

void UTimeTravel::BeginPlay()
{
	Super::BeginPlay();
	SnapshotTriggered.AddDynamic(this,&UTimeTravel::RecordSnapshot);
	SnapshotTriggered.Broadcast(0.4f);
}


void UTimeTravel::RecordSnapshot(float DeltaTime)
{
	UE_LOG(LogTemp, Display, TEXT("%s"),*GetOwner()->GetTransform().GetLocation().ToString());
}

UTimeTravel::~UTimeTravel()
{
	
}
