// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ObservationBox.h"
#include "DrawDebugHelpers.h"
#include "Logic/Util/UtilObservation.h"
#include <Engine\Classes\Components\BoxComponent.h>

static TAutoConsoleVariable< int32 > CVarRLDebugDrawObservationBox(
	TEXT( "RL.DebugDrawObservationBox" ),
	0,
	TEXT( "Draw shape for debugging (0:Ignore, 1:Draw)" ),
	ECVF_SetByCode );

// Sets default values
AObservationBox::AObservationBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AObservationBox::Reset()
{
	Wall = 0;
	Enemy = 0;
	Projectile = 0;
}

// Called every frame
void AObservationBox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ( BoxCollisionComp.IsValid() )
	{
		static const auto cvbarRLDebugDrawObservationBox = IConsoleManager::Get().FindConsoleVariable( TEXT( "RL.DebugDrawObservationBox" ) );
		if ( cvbarRLDebugDrawObservationBox && 0 < cvbarRLDebugDrawObservationBox->GetInt() )
		{
			FColor color( 50, 50, 50 );
			if ( Wall > 0 )
			{
				color.R = 255;
			}
			if ( Enemy > 0 )
			{
				color.B = 255;
			}
			if ( Projectile > 0 )
			{
				color.G = 255;
			}
			
			DrawDebugBox( UtilObservation::GetWorldSafe(), BoxCollisionComp->GetComponentTransform().GetLocation(), BoxCollisionComp->GetScaledBoxExtent(), GetActorRotation().Quaternion(), color, false, 0.1f, 0, 1.f );
		}
	}
	
}

// 사이즈를 변경한다.
void AObservationBox::SetSize( float X, float Y )
{
	if ( BoxCollisionComp.IsValid() )
	{
		FVector boxSize = BoxCollisionComp->GetUnscaledBoxExtent();
		boxSize.X = X;
		boxSize.Y = Y;
		BoxCollisionComp->SetBoxExtent( boxSize );
	}
}

// Called when the game starts or when spawned
void AObservationBox::BeginPlay()
{
	Super::BeginPlay();

	BoxCollisionComp = FindComponentByClass<UBoxComponent>();
	if ( BoxCollisionComp.IsValid() )
	{
		BoxCollisionComp->OnComponentBeginOverlap.RemoveDynamic( this, &AObservationBox::OnCollisionBeginOverlap );
		BoxCollisionComp->OnComponentEndOverlap.RemoveDynamic( this, &AObservationBox::OnCollisionEndOverlap );

		BoxCollisionComp->OnComponentBeginOverlap.AddDynamic( this, &AObservationBox::OnCollisionBeginOverlap );
		BoxCollisionComp->OnComponentEndOverlap.AddDynamic( this, &AObservationBox::OnCollisionEndOverlap );
	}
	
}

// 오버랩 시작
void AObservationBox::OnCollisionBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverLapInfo )
{
	if ( !OtherActor ) return;	
	
	if ( OtherActor->Tags.Find( "Enemy" ) )
	{
		Enemy += 1;
	}
	else if ( OtherActor->Tags.Find( "Wall" ) )
	{
		Wall += 1;
	}
	else if ( OtherActor->Tags.Find( "Projectile" ) )
	{
		Projectile += 1;
	}
	
	//if ( root->ComponentTags() )
	//{

	//}
	// 
	//if (root->ComponentHasTag("StageFloor"))            
	//{
	//	//현재 i번쨰 액터를 AStageFloor로 Cast해서 받아온다.                
	//	stagefloor = Cast<AStageFloor>(actors[i]);
	//}        
}

// 오버랩 끝
void AObservationBox::OnCollisionEndOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	if ( !OtherActor ) return;

	if ( OtherActor->Tags.Find( "Enemy" ) )
	{
		Enemy -= 1;
	}
	else if ( OtherActor->Tags.Find( "Wall" ) )
	{
		Wall -= 1;
	}
	else if ( OtherActor->Tags.Find( "Projectile" ) )
	{
		Projectile -= 1;
	}
}
