// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravelGlobal.h"
#include "kismet/GameplayStatics.h"
#include "Saoirse.h"

ATimeTravelGlobal::ATimeTravelGlobal()
{
	DefaultPawnClass= ASaoirse::StaticClass();
	// Set HUD class
	HUDClass = ASovrinHUD::StaticClass();

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
			TimeTravelActorsInWorld.Add(Actor->FindComponentByClass<UTimeTravel>());
		}
	}
}

void ATimeTravelGlobal::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

ATimeTravelGlobal::~ATimeTravelGlobal()
{
	
}
