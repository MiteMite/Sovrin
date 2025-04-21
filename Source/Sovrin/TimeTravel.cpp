// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravel.h"

#include "ComponentUtils.h"
#include "MaterialHLSLTree.h"

UTimeTravel::UTimeTravel()
{
	PrimaryComponentTick.bCanEverTick=true;
	
}

void UTimeTravel::RecordSnapshot(float DeltaTime)
{
	TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
	UE_LOG(LogTemp,Warning,TEXT("%s"),*TransformAndVelocitySnapshots.First().Transform.ToString());
}

UTimeTravel::~UTimeTravel()
{
	
}
