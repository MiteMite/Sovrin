// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravel.h"

#include "ComponentUtils.h"
#include "MaterialHLSLTree.h"
#include "TimeTravelGlobal.h"

UTimeTravel::UTimeTravel()
{
	PrimaryComponentTick.bCanEverTick=true;
	
}

void UTimeTravel::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RecordSnapshot(DeltaTime);
}

void UTimeTravel::BeginPlay()
{
	Super::BeginPlay();
	SnapshotTriggered.AddDynamic(this,&UTimeTravel::RecordSnapshot);
	SnapshotTriggered.Broadcast(0.4f);
}


void UTimeTravel::RecordSnapshot(float DeltaTime)
{
	if (!TransformAndVelocitySnapshots.IsEmpty())
	{
		TransformAndVelocitySnapshots.First().TimeSinceLastSnapshot+=DeltaTime;
		if (TransformAndVelocitySnapshots.First().TimeSinceLastSnapshot>=(1.0f/30.0f))//record at 30 frames per second
		{
			if (TransformAndVelocitySnapshots.Num()>50)//ensure that the ring buffer does not go over a certain size
			{
				TransformAndVelocitySnapshots.RemoveAt(0);
				UE_LOG(LogTemp, Display, TEXT("Removing oldest snapshot"));
			}
			else
			{
				TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
				UE_LOG(LogTemp, Display, TEXT("Adding new snapshot after %f"),TransformAndVelocitySnapshots.Last().TimeSinceLastSnapshot);
				TransformAndVelocitySnapshots.First().TimeSinceLastSnapshot=0.0f;
			}
		}
	}
	else
	{
		TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
		UE_LOG(LogTemp, Display, TEXT("Adding new snapshot if buffer is empty"));
	}
}

UTimeTravel::~UTimeTravel()
{
	
}
