
#pragma once


#include "CoreMinimal.h"
#include <Containers/Union.h>
#include <UObject/ObjectMacros.h>


class AActor;
class AObservationBox;
class UBoxComponent;


typedef TWeakObjectPtr< AActor > FActorPtr;
typedef TWeakObjectPtr< AObservationBox > FObservationBoxPtr;
typedef TWeakObjectPtr< UBoxComponent > FBoxComponentPtr;

//-------------------------------------------------------------------------------
// Enum <->String 변환함수
//-------------------------------------------------------------------------------

// Enum -> String
template< typename T >
FString EnumToString( const T value )
{
	UEnum* enumClass = StaticEnum< T >();
	if ( !ensureAlways( enumClass ) ) return TEXT( "" );

	return enumClass->GetNameStringByValue( ( int64 ) value );
}

// String -> Enum
template< typename T >
T StringToEnum( const TCHAR* String, T DefaultValue )
{
	if ( UEnum* Enum = StaticEnum< T >() )
	{
		int32 idx = Enum->GetValueByName( FName( String ) );
		return ( idx != INDEX_NONE ) ? ( T ) idx : DefaultValue;
	}
	else
	{
		DefaultValue;
	}
}

// String -> Enum
template< typename T >
T StringToEnum( const TCHAR* String )
{
	return StringToEnum< T >( String, T::MAX );
}

#define ENUM_TO_STRING( TYPE, VAL )                 EnumToString< TYPE >( VAL )
#define STRING_TO_ENUM( TYPE, STR )                 StringToEnum< TYPE >( STR, TYPE::Max )
#define STRING_TO_ENUM_DV( TYPE, STR, DEFAULT_VAL ) StringToEnum< TYPE >( STR, DEFAULT_VAL )


UENUM()
enum class ERLAgentType : uint8
{
	Dqn,
	Rainbow,
	Implicit_quantile,
	MAX
};