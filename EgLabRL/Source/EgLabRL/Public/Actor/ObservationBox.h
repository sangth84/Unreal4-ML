// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/RLType.h"
#include <Engine\TriggerBase.h>
#include "ObservationBox.generated.h"

class UBoxComponent;

UCLASS()
class AObservationBox : public AActor
{
	GENERATED_BODY()

private:
	FBoxComponentPtr BoxCollisionComp; // �ڽ� �ݸ���
	int32            Index;         // �ε���
	int32            Wall;          // ��
	int32            Enemy;         // ��
	int32            Projectile;    // �߻�ü
	
public:	
	// Sets default values for this actor's properties
	AObservationBox();

	// �ʱ�ȭ�Ѵ�.
	void Reset();

	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// �ε����� �����Ѵ�.
	void SetIndex( int32 Value ) { Index = Value; }

	// �ε����� ��ȯ�Ѵ�.
	int32 GetIndex() { return Index; }

	// ����� �����Ѵ�.
	void SetSize( float X, float Y );

	// �� ���ڸ� ��ȯ�Ѵ�.
	int32 GetWallCount() { return Wall; }

	// �� ���ڸ� ��ȯ�Ѵ�.
	int32 GeEnemyCount() { return Enemy; }

	// �߻�ü ���ڸ� ��ȯ�Ѵ�.
	int32 GetProjectileCount() { return Projectile; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// ������ ����
	UFUNCTION()
		void OnCollisionBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverLapInfo );

	// ������ ��
	UFUNCTION()
		void OnCollisionEndOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

};
