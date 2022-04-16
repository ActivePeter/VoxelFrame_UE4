// Fill out your copyright notice in the Description page of Project Settings.


#include "game/VF_EntityBase.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AVF_EntityBase::AVF_EntityBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVF_EntityBase::BeginPlay()
{
	Super::BeginPlay();

	//this->GetCapsuleComponent()->SetEnableGravity(true);
	//this->GetCapsuleComponent()->SetSimulatePhysics(true);
}

// Called every frame
void AVF_EntityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVF_EntityBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

