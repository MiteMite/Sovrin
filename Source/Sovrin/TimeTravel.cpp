// Fill out your copyright notice in the Description page of Project Settings.
#include "TimeTravel.h"
#include "ComponentUtils.h"
#include "Sovrin/Saoirse.h"
#include "Engine/World.h"
#include "TimeTravelGlobal.h"
#include "Kismet/GameplayStatics.h"


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
	//setup delegate bindings
	OnTimeTravelStarted.AddDynamic(this,&UTimeTravel::FOnTimeTravelStarted);
	OnTimeTravelEnded.AddDynamic(this,&UTimeTravel::FOnTimeTravelEnded);
}

void UTimeTravel::CallOnTimeTravelStarted()
{
	OnTimeTravelStarted.Broadcast();
}
void UTimeTravel::CallOnTimeTravelEnded()
{
	OnTimeTravelEnded.Broadcast();
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
				if (TransformAndVelocitySnapshots.Num()>=3000)//ensure that the ring buffer does not go over a certain size
				{
					TransformAndVelocitySnapshots.RemoveAt(0);
				}
				else
				{
					TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
				}
			}
		}
		else
		{
			TransformAndVelocitySnapshots.Add(FTransformAndVelocitySnapshot(DeltaTime,GetOwner()->GetTransform(),GetOwner()->GetVelocity()));
		}
	}
}

void UTimeTravel::PlaySnapshots(float DeltaTime, bool bRewinding)
{
	if (!TransformAndVelocitySnapshots.IsEmpty())
	{
		if (bRewinding)
		{
			{
				PlaybackSpeed+=DeltaTime;
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
	//UE_LOG(LogTemp, Display, TEXT("Started Rewinding time"));

	IsRewinding = true;
	IsFastForwarding = false;
	IsTimeScrubbing = false;
}

void UTimeTravel::FOnTimeTravelEnded()
{
	//UE_LOG(LogTemp, Display, TEXT("Stopped Rewinding time"));

	IsRewinding = false;
}

void UTimeTravel::PauseTime(float DeltaTime, bool bRewinding)
{
	
}

UTimeTravel::~UTimeTravel()
{
	
}
