// Fill out your copyright notice in the Description page of Project Settings.

#include "RLController.h"
#include "BP_Player_Base.h"
#include "Engine.h"
#include "AssertionMacros.h"

ARLController::ARLController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ARLController::BeginPlay()
{
	BP_Player_Obj = Cast<ABP_Player_Base>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	BP_Player_Obj->LeftMouseDown = true;
}

void ARLController::ProcessMove(int moveInput)
{
	RL_Direction::Type moveDirectionType = (RL_Direction::Type)moveInput;
	FVector vecMoveDirection(0, 0, 0);
	switch (moveDirectionType)
	{
	case RL_Direction::LEFT:
		vecMoveDirection.X = -1;
		break;
	case RL_Direction::RIGHT:
		vecMoveDirection.X = 1;
		break;
	case RL_Direction::DOWN:
		vecMoveDirection.Y = 1;
		break;
	case RL_Direction::UP:
		vecMoveDirection.Y = -1;
		break;
	case RL_Direction::LEFT_UP:
		vecMoveDirection.X = -1;
		vecMoveDirection.Y = -1;
		break;
	case RL_Direction::RIGHT_UP:
		vecMoveDirection.X = 1;
		vecMoveDirection.Y = -1;
		break;
	case RL_Direction::LEFT_DOWN:
		vecMoveDirection.X = -1;
		vecMoveDirection.Y = 1;
		break;
	case RL_Direction::RIGHT_DOWN:
		vecMoveDirection.X = 1;
		vecMoveDirection.Y = 1;
		break;
	case RL_Direction::STAY:
		break;
	default:
		checkf(false, TEXT("invalid value, moveInput : %d"), moveInput)
	}

	BP_Player_Obj->LeftMouseDown = true;
	BP_Player_Obj->AddMovementInput(vecMoveDirection);
}

void ARLController::ProcessRotation(int rotationInput)
{
	RL_Direction::Type rotationDirectionType = (RL_Direction::Type)rotationInput;
	FVector vecRotationDirection(0, 0, 0);
	switch (rotationDirectionType)
	{
	case RL_Direction::LEFT:
		vecRotationDirection.X = -1;
		break;
	case RL_Direction::RIGHT:
		vecRotationDirection.X = 1;
		break;
	case RL_Direction::DOWN:
		vecRotationDirection.Y = 1;
		break;
	case RL_Direction::UP:
		vecRotationDirection.Y = -1;
		break;
	case RL_Direction::LEFT_UP:
		vecRotationDirection.X = -1;
		vecRotationDirection.Y = -1;
		break;
	case RL_Direction::RIGHT_UP:
		vecRotationDirection.X = 1;
		vecRotationDirection.Y = -1;
		break;
	case RL_Direction::LEFT_DOWN:
		vecRotationDirection.X = -1;
		vecRotationDirection.Y = 1;
		break;
	case RL_Direction::RIGHT_DOWN:
		vecRotationDirection.X = 1;
		vecRotationDirection.Y = 1;
		break;
	case RL_Direction::STAY:
		break;
	default:
		checkf(false, TEXT("invalid value, rotationInput : %d"), rotationInput)
	}
	BP_Player_Obj->MouseAimVector = vecRotationDirection;
	//BP_Player_Obj->StickMoving = true;
	BP_Player_Obj->RotateWeapon();	
}

void ARLController::ChangeWeapon(int changeWeaponInput)
{
	if (changeWeaponInput == 1)
	{
		BP_Player_Obj->ActionNextWeapon();
	}
}
