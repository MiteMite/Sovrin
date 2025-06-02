// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravelGlobal.h"
#include "kismet/GameplayStatics.h"
#include "Saoirse.h"

ATimeTravelGlobal::ATimeTravelGlobal()
{
	DefaultPawnClass= ASaoirse::StaticClass();
}

void ATimeTravelGlobal::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(World,AActor::StaticClass(),ActorsInWorld);
	for (AActor* Actor : ActorsInWorld)
	{
		if (Actor->FindComponentByClass<UTimeTravel>()!=nullptr)
		{
			//UE_LOG(LogTemp,Warning,TEXT("This actor is: %s"),*Actor->GetName());
			TimeTravelActorsInWorld.Add(Actor->FindComponentByClass<UTimeTravel>());
		}
	}
}

ATimeTravelGlobal::~ATimeTravelGlobal()
{
	
}
