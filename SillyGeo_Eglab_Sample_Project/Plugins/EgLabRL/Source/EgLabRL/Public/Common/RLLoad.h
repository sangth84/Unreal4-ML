
#pragma once

class UClass;

template < typename T >
inline UClass* RLLoadClass( const TCHAR* Path )
{
	FString PathName( Path );

	int32 PackageDelimPos = INDEX_NONE;
	PathName.FindChar( TCHAR( '.' ), PackageDelimPos );
	if ( PackageDelimPos == INDEX_NONE )
	{
		int32 ObjectNameStart = INDEX_NONE;
		PathName.FindLastChar( TCHAR( '/' ), ObjectNameStart );
		if ( ObjectNameStart != INDEX_NONE )
		{
			const FString ObjectName = PathName.Mid( ObjectNameStart + 1 );
			PathName += TCHAR( '.' );
			PathName += ObjectName;
			PathName += TCHAR( '_' );
			PathName += TCHAR( 'C' );
		}
	}
	UClass* loadedClass = LoadClass<T>( nullptr, *PathName );
	return loadedClass;
}

template < typename T >
inline UClass* RLLoadObject( const TCHAR* Path )
{
	FString PathName( Path );

	int32 PackageDelimPos = INDEX_NONE;
	PathName.FindChar( TCHAR( '.' ), PackageDelimPos );
	if ( PackageDelimPos == INDEX_NONE )
	{
		int32 ObjectNameStart = INDEX_NONE;
		PathName.FindLastChar( TCHAR( '/' ), ObjectNameStart );
		if ( ObjectNameStart != INDEX_NONE )
		{
			const FString ObjectName = PathName.Mid( ObjectNameStart + 1 );
			PathName += TCHAR( '.' );
			PathName += ObjectName;			
		}
	}
	UClass* Class = T::StaticClass();
	Class->GetDefaultObject(); // force the CDO to be created if it hasn`t already
	T* ObjectPtr = LoadObject<T>( nullptr, *PathName );
	return ObjectPtr;
}
