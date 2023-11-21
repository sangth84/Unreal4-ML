// Fill out your copyright notice in the Description page of Project Settings.

#include "EglabManager.h"
#include "Actor/ObservationBox.h"
#include "Component/ObservationComponent.h"
#include "Engine.h"
#include "Logic/RLControllerInterface.h"
#include "MachineLearningRemoteComponent.h"
#include "VisualLogger/VisualLogger.h"


#define MAX_ENEMY 50
#define MAX_MAP_SIZE 2400.0f
#define MAX_WEAPON_COUNT 4.0f
#define MAX_SPEED 1000.0f
#define MAX_OBSERVATION 8


// Sets default values
AEglabManager::AEglabManager()
:
ObservationSize( 50.f, 50.f ),
WidthCount( 10 ),
VerticalCount( 10 ),
OffSetPos( 0.f, 0.f ),
RLAgentType( ERLAgentType::Rainbow ),
fReward(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyArray.Reset(MAX_ENEMY);	
	//ObservationArray.Reset(( MAX_ENEMY + 1 ) * 4);
	ObservationArray.Reset( ( WidthCount * VerticalCount ) * MAX_OBSERVATION );
	Tags.Add(TEXT("TF"));
	bDone = false;

	ObservationComponentPtr = CreateDefaultSubobject< UObservationComponent >( "Observation" );
	
}

// Called when the game starts or when spawned
void AEglabManager::BeginPlay()
{
	if ( UMachineLearningRemoteComponent* MLComponent = FindComponentByClass<UMachineLearningRemoteComponent>() )
	{
		MLComponent->SetObservationSpace( ( WidthCount * VerticalCount ) * MAX_OBSERVATION );
	}

	Super::BeginPlay();

	BP_Player_Obj = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	RLControllerObj = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if ( RLControllerObj && ObservationComponentPtr.IsValid() )
	{		
		if ( AActor* actor = Cast<AActor>( RLControllerObj->GetCharacter() ) )
		{
			ObservationComponentPtr->ObservationSize = ObservationSize;
			ObservationComponentPtr->WidthCount = WidthCount;
			ObservationComponentPtr->VerticalCount = VerticalCount;
			ObservationComponentPtr->OffSetPos = OffSetPos;
			
			ObservationComponentPtr->InitObservationActor( actor );
		}
	}
}

// Called every frame
void AEglabManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEglabManager::GiveReward()
{
	fReward += 0.5f;
	//UE_VLOG(this, "TF", Log, TEXT("Give Reward : %f"), Reward);
}

void AEglabManager::AddEnemyNotiyfication(AActor * EnemyObj, int32 WeakType, float EnemySpeed)
{
	EnemyArray.Emplace(EnemyObj);
	EnemyWeakTypeArray.Emplace(WeakType);
	EnemySpeedArray.Emplace(EnemySpeed);
	check(EnemyArray.Num() < MAX_ENEMY);

	//UE_VLOG(this, "TF", Log, TEXT("ADD Enemy : %d"), EnemyArray.Num());
}

void AEglabManager::DestoryEnemyNotiyfication(AActor * EnemyObj)
{
	int32 index;
	if (EnemyArray.Find(EnemyObj, index))
	{
		EnemyArray.RemoveAt(index);
		EnemyWeakTypeArray.RemoveAt(index);
		EnemySpeedArray.RemoveAt(index);
	}
	else
	{
		UE_VLOG(this, "TF", Log, TEXT("Don`t Destory Enemy"));
	}
	//UE_VLOG(this, "TF", Log, TEXT("Destory Enemy : %d"), EnemyArray.Num());
}

void AEglabManager::EndGameNotiyfication()
{
	EnemyArray.Empty();
	EnemyWeakTypeArray.Empty();
	EnemySpeedArray.Empty();
	bDone = true;
	//UE_VLOG(this, "TF", Log, TEXT("End Game"));
}

void AEglabManager::GetAIInputResult(TArray<float>& observation, float & reward, bool & done)
{
	observation = GetObservationArry();
	reward = fReward;
	done = bDone;
}

void AEglabManager::SetAIInput(TArray<int32> input)
{
	fReward = .0f;
	bDone = false;

	check(input.Num() == 3);
	if ( IRLControllerInterface* interface = Cast< IRLControllerInterface >( RLControllerObj ) )
	{
		interface->ProcessMove( input[ 0 ] );
		interface->ProcessRotation( input[ 1 ] );
		interface->ChangeWeapon( input[ 2 ] );
	}	
}

TArray<float> AEglabManager::GetObservationArry()
{
	ObservationArray.Empty();
	ObservationArray.Init( 0, ( WidthCount * VerticalCount ) * MAX_OBSERVATION );

	if ( BP_Player_Obj == NULL || !ObservationComponentPtr.IsValid() ) return ObservationArray;
	
	float halfWidthCount = WidthCount / 2.f;
	float halfVerticalCount = VerticalCount / 2.f;
	
	int posision = 0;
	for ( float VerticalIndex = 0; VerticalIndex < VerticalCount; ++VerticalIndex )
	{
		for ( float WidthIndex = 0; WidthIndex < WidthCount; ++WidthIndex )
		{
			if ( AObservationBox* box = ObservationComponentPtr->GetObservationBox( WidthIndex + WidthCount * VerticalIndex ) )
			{
				ObservationArray[ posision++ ] = ( WidthIndex - halfWidthCount ) / halfWidthCount;
				ObservationArray[ posision++ ] = ( VerticalIndex - halfVerticalCount ) / halfVerticalCount;
				ObservationArray[ posision++ ] = FVector::Dist2D( BP_Player_Obj->GetActorLocation(), box->GetActorLocation() );
				ObservationArray[ posision++ ] = box->GetWallCount();
				ObservationArray[ posision++ ] = box->GeEnemyCount();
				ObservationArray[ posision++ ] = box->GetProjectileCount();
				ObservationArray[ posision++ ] = 0;
				ObservationArray[ posision++ ] = 0;
			}			
		}
	}
	
	return ObservationArray;
}

