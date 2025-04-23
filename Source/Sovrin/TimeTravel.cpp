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
	PlaySnapshots(DeltaTime,IsRewinding);
}

void UTimeTravel::BeginPlay()
{
	Super::BeginPlay();
	OnTimeTravelStarted.AddDynamic(this,&UTimeTravel::FOnTimeTravelStarted);
	OnTimeTravelEnded.AddDynamic(this,&UTimeTravel::FOnTimeTravelEnded);
}


void UTimeTravel::RecordSnapshot(float DeltaTime)
{
	if (!IsRewinding)
	{
		if (!TransformAndVelocitySnapshots.IsEmpty())
		{
			TransformAndVelocitySnapshots.First().TimeSinceLastSnapshot+=DeltaTime;
			if (TransformAndVelocitySnapshots.First().TimeSinceLastSnapshot>=(1.0f/30.0f))//record at 30 frames per second
			{
				if (TransformAndVelocitySnapshots.Num()>=1000)//ensure that the ring buffer does not go over a certain size
				{
					TransformAndVelocitySnapshots.RemoveAt(0);
					//UE_LOG(LogTemp, Display, TEXT("Removing oldest snapshot"));
				}
				else
				{
					TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
					//UE_LOG(LogTemp, Display, TEXT("Adding new snapshot after %f"),TransformAndVelocitySnapshots.Last().TimeSinceLastSnapshot);
					//TransformAndVelocitySnapshots.First().TimeSinceLastSnapshot=0.0f;
				}
			}
		}
		else
		{
			TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
			//UE_LOG(LogTemp, Display, TEXT("Adding new snapshot if buffer is empty"));
		}
	}
}

void UTimeTravel::PlaySnapshots(float DeltaTime, bool bRewinding)
{
	PlaybackSpeed+=DeltaTime;
	if (!TransformAndVelocitySnapshots.IsEmpty())
	{
		//UE_LOG(LogTemp, Display, TEXT("Rewinding is %f"),PlaybackSpeed);
		if (bRewinding)
		{
			{
				UE_LOG(LogTemp, Display, TEXT("Rewinding to %f"),PlaybackSpeed);
				if (PlaybackSpeed>0.003f)
				{
					this->GetOwner()->SetActorTransform(TransformAndVelocitySnapshots.Last().Transform,true,nullptr,ETeleportType::ResetPhysics);
					TransformAndVelocitySnapshots.Pop();
					PlaybackSpeed=0.0f;
				}
			}
		}
	}
}

void UTimeTravel::FOnTimeTravelStarted()
{
	IsRewinding = true;
	IsFastForwarding = false;
	IsTimeScrubbing = false;
}

void UTimeTravel::FOnTimeTravelEnded()
{
	IsRewinding = false;
}

void UTimeTravel::PauseTime(float DeltaTime, bool bRewinding)
{
	
}

UTimeTravel::~UTimeTravel()
{
	
}
