// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Logic/RLControllerInterface.h"
#include "RLController.generated.h"

namespace RL_Direction
{
	enum Type
	{
		STAY,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		LEFT_UP,
		RIGHT_UP,
		LEFT_DOWN,
		RIGHT_DOWN,
		MAX
	};
}


class ABP_Player_Base;
/**
 *
 */
UCLASS()
class SILLYGEO_API ARLController : public APlayerController, public IRLControllerInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Player)
		ABP_Player_Base* BP_Player_Obj = NULL;

protected:
	ARLController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

public:

	virtual void ProcessMove(int moveInput) override;
	virtual void ProcessRotation(int rotationInput) override;
	virtual void ChangeWeapon(int changeWeaponInput) override;
};
