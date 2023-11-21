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
	// Observation ũ��
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		FVector2D ObservationSize;

	// ���� �迭 ��
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		int32 WidthCount;

	// ���� �迭 ��
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		int32 VerticalCount;

	// Observation ũ��
	UPROPERTY( BlueprintReadOnly, Category = "RL" )
		FVector2D OffSetPos;

public:
	// ������
	UObservationComponent();

	// �ʱ�ȭ�Ѵ�.
	void Reset();

	// ���� ���͸� �����Ѵ�.
	void InitObservationActor( AActor* Target );

	// ���� ��
	virtual void BeginPlay() override;

	// ���� ��
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	// ���� �ڽ��� ��ȯ�Ѵ�.
	AObservationBox* GetObservationBox( int Index );

private:
	// ���� ����
	AObservationBox* _SpawnObservationActor( AActor* Target, int32 WidthIndex, int32 VerticalIndex );

	// ���� ��ġ�� ��ȯ�Ѵ�.
	FVector _SpawnPosition( int32 WidthIndex, int32 VerticalIndex );
};