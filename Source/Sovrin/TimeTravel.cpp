// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeTravel.h"

UTimeTravel::UTimeTravel()
{
}

void UTimeTravel::BeginPlay()
{
	UE_LOG(LogTemp,Warning,TEXT("Hello world"));
	for (int i = 0;i<10;i++)
	{
		UE_LOG(LogTemp,Warning,TEXT("Hello world %i"),i); 
	}
}

UTimeTravel::~UTimeTravel()
{
	
}
