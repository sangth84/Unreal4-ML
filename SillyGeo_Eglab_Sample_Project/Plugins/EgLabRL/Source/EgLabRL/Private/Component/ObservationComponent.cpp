
#include "ObservationComponent.h"
#include "Actor/ObservationBox.h"
#include "Common/RLLoad.h"
#include "Logic/Util/UtilObservation.h"

UObservationComponent::UObservationComponent()
:
ObservationSize ( 50.f, 50.f ),
WidthCount      ( 10 ),
VerticalCount   ( 10 ),
OffSetPos       ( 0.f, 0.f )
{

}

void UObservationComponent::Reset()
{
	for ( FObservationBoxPtr& observationBox : FObservationBoxArray )
	{
		if ( !observationBox.IsValid() ) continue;

		observationBox->Reset();
	}
}

void UObservationComponent::InitObservationActor( AActor* Target )
{
	if ( !Target ) return;

	for ( int32 verticalIndex = 0; verticalIndex < VerticalCount; ++verticalIndex )
	{
		for ( int32 widthIndex = 0; widthIndex < WidthCount; ++widthIndex )
		{
			_SpawnObservationActor( Target, widthIndex, verticalIndex );
		}
	}
}

void UObservationComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UObservationComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );
}

AObservationBox* UObservationComponent::GetObservationBox( int Index )
{
	if ( !FObservationBoxArray.IsValidIndex( Index ) ) return nullptr;

	ensure( FObservationBoxArray[ Index ]->GetIndex() == Index );
	return FObservationBoxArray[ Index ].Get();
}

AObservationBox* UObservationComponent::_SpawnObservationActor( AActor* Target, int32 WidthIndex, int32 VerticalIndex )
{	
	const FString& path =  UtilObservation::GetBPPath( TEXT( "BP_ObservationBox" ) );
	UClass* uclass = RLLoadClass< AActor >( *path );

	if ( !uclass ) return nullptr;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector& spawnPosition = _SpawnPosition( WidthIndex, VerticalIndex );

	AObservationBox* observationBox = GetWorld()->SpawnActor<AObservationBox>( uclass , spawnPosition, FRotator::ZeroRotator, SpawnParameters );
	if ( !observationBox ) return nullptr;

	observationBox->AttachToActor( Target, FAttachmentTransformRules::KeepRelativeTransform );
	observationBox->SetIndex( WidthIndex + WidthCount * VerticalIndex );
	observationBox->SetSize( ObservationSize.X, ObservationSize.Y );

	FObservationBoxArray.Add( observationBox );

	ensure( observationBox->GetIndex() == ( FObservationBoxArray.Num() - 1 ) );

	return observationBox;
}

FVector UObservationComponent::_SpawnPosition( int32 WidthIndex, int32 VerticalIndex )
{
	FVector position( OffSetPos.X, OffSetPos.Y, 0.f );

	int32 widthNum = WidthIndex - WidthCount / 2;
	if ( WidthCount % 2 )
	{
		position.X += widthNum * ObservationSize.X * 2;
	}
	else
	{
		if ( widthNum >= 0 )
		{
			position.X += widthNum * ObservationSize.X * 2;
		}
		else
		{
			position.X += widthNum * ObservationSize.X * 2;
		}
	}

	int32 verticalNum = VerticalIndex - VerticalCount / 2;
	if ( VerticalCount % 2 )
	{
		position.Y += verticalNum * ObservationSize.Y * 2;
	}
	else
	{
		if ( verticalNum >= 0 )
		{
			position.Y += verticalNum * ObservationSize.Y * 2;
		}
		else
		{
			position.Y += verticalNum * ObservationSize.Y * 2;
		}
	}

	return position;
}

