// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BP_Player_Base.generated.h"

UCLASS()
class SILLYGEO_API ABP_Player_Base : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABP_Player_Base();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		FVector StickAimVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		FVector MouseAimVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		bool StickMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		bool LeftMouseDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		float Health = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		float MaximumHealth = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ref_Component")
		int32 CurrentWeapon = 0;

	UFUNCTION(BlueprintImplementableEvent, Category = "Functions")
		void RotateWeapon();

	UFUNCTION(BlueprintImplementableEvent, Category = "Functions")
		void ActionNextWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
