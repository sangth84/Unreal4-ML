#pragma once


#include "Common/RLType.h"
#include "Components/SceneComponent.h"
#include "ObservationComponent.generated.h"

class AObservationBox;

UCLASS()
class EGLABRL_API UObservationComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	TArray< FObservationBoxPtr > FObservationBoxArray;

public:
	// Observation 크기
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		FVector2D ObservationSize;

	// 가로 배열 수
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		int32 WidthCount;

	// 세로 배열 수
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		int32 VerticalCount;

	// Observation 크기
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		FVector2D OffSetPos;

public:
	// 생성자
	UObservationComponent();

	// 초기화한다.
	void Reset();

	// 관찰 액터를 생성한다.
	void InitObservationActor( AActor* Target );

	// 생성 시
	virtual void BeginPlay() override;

	// 종료 시
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	// 관찰 박스를 반환한다.
	AObservationBox* GetObservationBox( int Index );

private:
	// 액터 스폰
	AObservationBox* _SpawnObservationActor( AActor* Target, int32 WidthIndex, int32 VerticalIndex );

	// 생성 위치를 반환한다.
	FVector _SpawnPosition( int32 WidthIndex, int32 VerticalIndex );
};