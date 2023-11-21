// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/RLType.h"
#include "GameFramework/Actor.h"
#include "EglabManager.generated.h"

class UObservationComponent;

UCLASS()
class EGLABRL_API AEglabManager : public AActor
{
	GENERATED_BODY()

private:
	typedef TWeakObjectPtr< UObservationComponent > FObservationComponentPtr;
	
public:	
	// Sets default values for this actor's properties
	AEglabManager();

public:
	// Observation 크기
	UPROPERTY( EditAnywhere, Category = "RL" )
		FVector2D ObservationSize;

	// 가로 배열 수
	UPROPERTY( EditAnywhere, Category = "RL" )
		int32 WidthCount;

	// 세로 배열 수
	UPROPERTY( EditAnywhere, Category = "RL" )
		int32 VerticalCount;

	// Observation 크기
	UPROPERTY( EditAnywhere, Category = "RL" )
		FVector2D OffSetPos;

	// Observation 크기
	UPROPERTY( EditAnywhere, Category = "RL" )
		ERLAgentType RLAgentType;

private:
	float fReward;
	TArray<int32> EnemyWeakTypeArray;
	TArray<float> EnemySpeedArray;
	TArray<float> ObservationArray;
	bool bDone;
	FObservationComponentPtr ObservationComponentPtr;

private:
	UPROPERTY()
		TArray<AActor*> EnemyArray;

	UPROPERTY()
		class ACharacter* BP_Player_Obj;

	UPROPERTY()
		class APlayerController* RLControllerObj;

public:
	UFUNCTION( BlueprintCallable, Category = "Functions" )
		void GiveReward();

	UFUNCTION( BlueprintCallable, Category = "Functions" )
		void AddEnemyNotiyfication( AActor* EnemyObj, int32 WeakType, float EnemySpeed );

	UFUNCTION( BlueprintCallable, Category = "Functions" )
		void DestoryEnemyNotiyfication( AActor* EnemyObj );

	UFUNCTION( BlueprintCallable, Category = "Functions" )
		void EndGameNotiyfication();

	UFUNCTION( BlueprintCallable, Category = "Functions" )
		void GetAIInputResult( TArray<float>& observation, float& reward, bool& done );

	UFUNCTION( BlueprintCallable, Category = "Functions" )
		void SetAIInput( TArray<int32> input );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	TArray<float> GetObservationArry();

};
