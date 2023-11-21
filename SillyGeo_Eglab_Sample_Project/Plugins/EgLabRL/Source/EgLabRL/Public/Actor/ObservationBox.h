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
	FBoxComponentPtr BoxCollisionComp; // 박스 콜리더
	int32            Index;         // 인덱스
	int32            Wall;          // 벽
	int32            Enemy;         // 적
	int32            Projectile;    // 발사체
	
public:	
	// Sets default values for this actor's properties
	AObservationBox();

	// 초기화한다.
	void Reset();

	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// 인덱스를 변경한다.
	void SetIndex( int32 Value ) { Index = Value; }

	// 인덱스를 반환한다.
	int32 GetIndex() { return Index; }

	// 사이즈를 변경한다.
	void SetSize( float X, float Y );

	// 벽 숫자를 반환한다.
	int32 GetWallCount() { return Wall; }

	// 적 숫자를 반환한다.
	int32 GeEnemyCount() { return Enemy; }

	// 발사체 숫자를 반환한다.
	int32 GetProjectileCount() { return Projectile; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// 오버랩 시작
	UFUNCTION()
		void OnCollisionBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverLapInfo );

	// 오버랩 끝
	UFUNCTION()
		void OnCollisionEndOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

};
