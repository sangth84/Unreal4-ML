
#include "UtilObservation.h"
#include "Engine/GameEngine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#include "Editor/UnrealEd/Public/EditorViewportClient.h"
#endif


// Get WorldContext
const FWorldContext* UtilObservation::GetWorldContext( TArray< EWorldType::Type > WorldTypes )
{
	if ( !GEngine ) return nullptr;

#if WITH_EDITOR

	const TIndirectArray< FWorldContext >& worldContextArray = GEngine->GetWorldContexts();
	for ( int32 index = 0; index < worldContextArray.Num(); ++index )
	{
		const FWorldContext& worldCtxt = worldContextArray[ index ];

		if ( WorldTypes.Find( worldCtxt.WorldType ) != INDEX_NONE )
		{
			return &worldCtxt;
		}
	}

#endif

	return nullptr;
}

#if WITH_EDITOR

// Get Viewport 
FEditorViewportClient* UtilObservation::GetViewportClientEd( UWorld* World )
{
	//UEditorEngine* engineEd = Cast< UEditorEngine >( GEngine );
	//if ( !engineEd ) return nullptr;

	//for ( int32 index = 0; index < engineEd->GetAllViewportClients().Num(); index++ )
	//{
	//	FEditorViewportClient* viewportClientEd = engineEd->GetAllViewportClients()[ index ];
	//	if ( !viewportClientEd ) continue;

	//	if ( World )
	//	{
	//		if ( viewportClientEd->GetWorld() == World )
	//		{
	//			return viewportClientEd;
	//		}
	//	}
	//	else
	//	{
	//		if ( UWorld* world = viewportClientEd->GetWorld() )
	//		{
	//			if ( world->HasBegunPlay() )
	//			{
	//				return viewportClientEd;
	//			}
	//		}
	//	}
	//}

	return nullptr;
}

#endif

// 월드를 반환한다.
UWorld* UtilObservation::GetWorldSafe( AActor* Actor )
{
	if ( Actor )
	{
		return Actor->GetWorld();
	}

#if WITH_EDITOR

	//if ( UEditorEngine* engineEd = Cast< UEditorEngine >( GEngine ) )
	//{
	//	if ( engineEd->PlayWorld )
	//	{
	//		return engineEd->PlayWorld;
	//	}

	//	if ( FEditorViewportClient* viewportClientEd = GetViewportClientEd() )
	//	{
	//		return viewportClientEd->GetWorld();
	//	}
	//}

#else

	if ( UGameEngine* gameEngine = Cast< UGameEngine >( GEngine ) )
	{
		return gameEngine->GameInstance->GetWorld();
	}

#endif

	if ( UGameInstance* gameInst = GetGameInst() )
	{
		return gameInst->GetWorld();
	}

	return nullptr;
}

FString UtilObservation::GetBPPath( const TCHAR* Name )
{
	const FString& outPut = FString::Printf( TEXT( "/EgLabRL/Blueprint/%s" ), Name );

	return outPut;
}
