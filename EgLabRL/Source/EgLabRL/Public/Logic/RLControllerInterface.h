#pragma once


#include "RLControllerInterface.generated.h"

UINTERFACE( Blueprintable )
class URLControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class IRLControllerInterface
{
	GENERATED_BODY()

public:
	virtual void ProcessMove( int moveInput ) = 0;
	virtual void ProcessRotation( int rotationInput ) = 0;
	virtual void ChangeWeapon( int changeWeaponInput ) = 0;
};