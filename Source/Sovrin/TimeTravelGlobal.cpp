// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravelGlobal.h"

#include "Saoirse.h"

ATimeTravelGlobal::ATimeTravelGlobal()
{
	DefaultPawnClass= ASaoirse::StaticClass();
}

void ATimeTravelGlobal::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> ActorsInWorld;
	UWorld* World = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(World,AActor::StaticClass(),ActorsInWorld);
	for (AActor* Actor : ActorsInWorld)
	{
		Actor->FindComponentByClass<UTimeTravel>()->SetupDelegates(Actor);
	}
}


ATimeTravelGlobal::~ATimeTravelGlobal()
{
	
}
