// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Player_Base.h"

// Sets default values
ABP_Player_Base::ABP_Player_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABP_Player_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_Player_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABP_Player_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

