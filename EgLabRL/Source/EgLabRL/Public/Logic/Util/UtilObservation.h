
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/GameEngine.h"
#include "Engine/GameInstance.h"
#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#endif

class UGameInstance;
class UWorld;
#if WITH_EDITOR
class FEditorViewportClient;
class UEditorEngine;
#endif


UENUM()
enum class EObservationType
{
	Enemy,

};

namespace UtilObservation
{
	// Get WorldContext
	const FWorldContext* GetWorldContext( TArray< EWorldType::Type > WorldTypes );

	// Get GameInstance
	template< typename T = UGameInstance >
	T* GetGameInst( TArray< EWorldType::Type > WorldTypes );

	// Get GameInstance
	template< typename T = UGameInstance >
	T* GetGameInst();
	
#if WITH_EDITOR

	// Get Viewport 
	FEditorViewportClient* GetViewportClientEd( UWorld* World = nullptr );

#endif

	// 월드를 반환한다.
	UWorld* GetWorldSafe( AActor* Actor = nullptr );

	// BP 패스 경로를 가져온다.
	FString GetBPPath( const TCHAR* Name );
}

// Get GameInstance
template< typename T >
T* UtilObservation::GetGameInst( TArray< EWorldType::Type > WorldTypes )
{
#if WITH_EDITOR

	if ( const FWorldContext* ctxt = GetWorldContext( WorldTypes ) )
	{
		return Cast< T >( ctxt->OwningGameInstance );
	}

#endif

	return nullptr;
}

template< typename T >
T* UtilObservation::GetGameInst()
{
	if ( !GEngine ) return nullptr;

#if WITH_EDITOR

	return GetGameInst< T >( { EWorldType::Game, EWorldType::PIE } );

#else

	UGameEngine* gameEngine = Cast< UGameEngine >( GEngine );
	if ( !gameEngine ) return nullptr;

	return Cast< T >( gameEngine->GameInstance );

#endif
}