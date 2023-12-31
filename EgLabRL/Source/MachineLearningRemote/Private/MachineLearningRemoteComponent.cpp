// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineLearningRemoteComponent.h"
#include "MachineLearningBase.h"
#include "SocketIOClient.h"
#include "CULambdaRunnable.h"
#include <Engine/Public/TimerManager.h>

bool UMachineLearningRemoteComponent::bServerIsRunning = false;
TSharedPtr<FMLProcess> UMachineLearningRemoteComponent::Process = nullptr;

UMachineLearningRemoteComponent::UMachineLearningRemoteComponent()
:
ObservationSpace ( 0 )
{
	PrimaryComponentTick.bCanEverTick = true;
	LatentAction = nullptr;

	bConnectOnBeginPlay = true;
	ServerType = ETFServerType::SERVER_PYTHON;
	ServerAddressAndPort = TEXT("http://localhost:8080");
	SendInputEventName = TEXT("sendInput");
	StartScriptEventName = TEXT("startScript");
	ScriptStartedEventName = TEXT("scriptStarted");
	LogEventName = TEXT("log");
	DefaultScript = TEXT("empty_example");
	bScriptRunning = false;
	bStartScriptOnConnection = true;

	bUseEmbeddedServer = false;
	bAutoStartServer = false;
	bPrintServerLog = true;
	EmbeddedServerRelativePath = TEXT("python3.7");	
}

UMachineLearningRemoteComponent::~UMachineLearningRemoteComponent()
{
	ISocketIOClientModule::Get().ReleaseNativePointer(Socket);
}

void UMachineLearningRemoteComponent::BeginPlay()
{
	Super::BeginPlay();

	/** autostart server if enabled */
	if (bAutoStartServer && !bServerIsRunning)
	{
		bServerIsRunning = true;
		FString PluginServerFolderPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("Plugins/machine-learning-remote-ue4/Server"));
		FString ServerScriptPath = FPaths::Combine( TEXT( "/Plugins/machine-learning-remote-ue4/Server" ), TEXT("ml-remote-server"));
		
		//UE_LOG(LogTemp, Log, TEXT("%s"), *ServerPath);
		if (bUseEmbeddedServer)
		{
			//FString EmbeddedServerPath = FPaths::Combine(TEXT("../"), EmbeddedServerRelativePath); //-p ServerScriptPath
			//Process = FMLProcess::Create(EmbeddedServerPath + TEXT("/python.exe"), ServerScriptPath + TEXT("/server.py"), true, false, false, 0, ServerScriptPath, true);
			FString EmbeddedServerPath = FPaths::Combine( PluginServerFolderPath, EmbeddedServerRelativePath );
			Process = FMLProcess::Create( EmbeddedServerPath + TEXT( "/python.exe" ), ServerScriptPath + TEXT( "/server.py" ), true, false, false, 0, ServerScriptPath, true );
		}
		else
		{
			//FString EmbeddedServerPath = FPaths::Combine( TEXT( "../" ), EmbeddedServerRelativePath ); //-p ServerScriptPath
			//Process = FMLProcess::Create( EmbeddedServerPath + TEXT("/python.exe"), TEXT("../ml-remote-server/server.py"), true, false, false, 0, ServerScriptPath, true);
			//FString EmbeddedServerPath = FPaths::Combine( PluginServerFolderPath, EmbeddedServerRelativePath );
			//Process = FMLProcess::Create( EmbeddedServerPath + TEXT( "/python.exe" ), ServerScriptPath + TEXT( "/server.py" ), true, false, false, 0, ServerScriptPath, true);
			Process = FMLProcess::Create( TEXT("C:/Unreal Projects/SillyGeo/Plugins/machine-learning-remote-ue4/Server/ml-remote-server/StartupEmbeddedServerTest"), TEXT(""), true, false, false, 2, TEXT(""), false);
		}
		
	}
	
	TargetServerConnect();	
}

void UMachineLearningRemoteComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Socket->Disconnect();
	Socket->ClearCallbacks();

	if (bServerIsRunning )
	{
		if ( Process )
		{
			Process->IsRunning();
			Process->Terminate();
			Process->Close();
			Process = nullptr;
		}		
		bServerIsRunning = false;
	}
	Super::EndPlay(EndPlayReason);
}

void UMachineLearningRemoteComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bServerIsRunning && Process.IsValid())
	{
		FString PipeData = Process->ReadFromPipe();

		if (!PipeData.IsEmpty() && bPrintServerLog)
		{
			UE_LOG(LogTemp, Log, TEXT("Server: %s"), *PipeData);
		}

		if (!Process->IsRunning())
		{
			int32 ReturnCode;
			Process->GetReturnCode(ReturnCode);
			UE_LOG(LogTemp, Log, TEXT("Server finished with: %d"), ReturnCode);

			Process = nullptr;
			bServerIsRunning = false;
		}
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMachineLearningRemoteComponent::SendSIOJsonInput(USIOJsonValue* InputData, USIOJsonValue*& ResultData, struct FLatentActionInfo LatentInfo, const FString& FunctionName /*= TEXT("onJsonInput")*/)
{
	if (!bIsConnectedToBackend) return;
	//Wrap input data with targeting information. Cannot formalize this as a struct unfortunately.
	auto SendObject = USIOJConvert::MakeJsonObject();
	SendObject->SetStringField(TEXT("targetFunction"), FunctionName);
	SendObject->SetField(TEXT("inputData"), InputData->GetRootValue());

	if (LatentAction)
	{
		LatentAction->Call();
		LatentAction = nullptr;

		Socket->Disconnect();
		ISocketIOClientModule::Get().ReleaseNativePointer(Socket);
		TargetServerConnect();

		return;
	}

	LatentAction = FCULatentAction::CreateLatentAction(LatentInfo, this);

	Socket->Emit(SendInputEventName, SendObject, [this, FunctionName, &ResultData](auto ResponseArray)
	{
		//UE_LOG(MLBaseLog, Log, TEXT("Got callback response: %s"), *USIOJConvert::ToJsonString(ResponseArray));
		if (ResponseArray.Num() == 0)
		{
			if ( LatentAction )
			{
				LatentAction->Call();
				LatentAction = nullptr;
			}
			return;
		}

		//We only handle the one response for now
		TSharedPtr<FJsonValue> Response = ResponseArray[0];

		ResultData = NewObject<USIOJsonValue>();

		ResultData->SetRootValue(Response);

		if ( LatentAction )
		{
			LatentAction->Call();	//resume the latent action
			LatentAction = nullptr;
			OnInputScriptResult.Broadcast(ResultData, FunctionName);
		}		
	});

	//GetWorld()->GetTimerManager().SetTimer(SendTimeHandler, []() {
	//	}, 0.1f );	
}

void UMachineLearningRemoteComponent::SendObservationSpace()
{
	if ( ObservationSpace == 0 ) return;

	Socket->Emit( TEXT("setObservationSpace"), FString::FromInt( ObservationSpace ) );
}


void UMachineLearningRemoteComponent::SendAgentType()
{
	if ( RLAgentType.IsEmpty() ) return;

	Socket->Emit( TEXT( "setAgentType" ), RLAgentType );
}

void UMachineLearningRemoteComponent::StartScript(const FString& ScriptName)
{
	Socket->Emit(StartScriptEventName, ScriptName);
	//todo get a callback when it has started?
}

void UMachineLearningRemoteComponent::SendStringInput(const FString& InputData, const FString& FunctionName /*= TEXT("onJsonInput")*/)
{
	const FString SafeFunctionName = FunctionName;
	SendStringInput(InputData, [this, SafeFunctionName](const FString& ResultData)
	{
		OnInputResult.Broadcast(ResultData, SafeFunctionName);
	}, FunctionName);
}

void UMachineLearningRemoteComponent::SendStringInput(const FString& InputData, TFunction<void(const FString& ResultData)> ResultCallback, const FString& FunctionName /*= TEXT("onJsonInput")*/)
{
	//Embed data in a ustruct, this will get auto-serialized into a python/json object on other side
	FMLSendStringObject SendObject;
	SendObject.InputData = InputData;
	SendObject.TargetFunction = FunctionName;

	const auto SafeCallback = ResultCallback;

	Socket->Emit(SendInputEventName, FMLSendStringObject::StaticStruct(), &SendObject, [this, FunctionName, SafeCallback](auto ResponseArray)
		{
			//UE_LOG(MLBaseLog, Log, TEXT("Got callback response: %s"), *USIOJConvert::ToJsonString(ResponseArray));
			if (ResponseArray.Num() == 0)
			{
				return;
			}

			//We only handle the one response for now
			TSharedPtr<FJsonValue> Response = ResponseArray[0];

			//Grab the value as a string
			//Todo: support non-string encoding?
			FString Result;

			if (Response->Type == EJson::String)
			{
				Result = Response->AsString();
			}
			else
			{
				Result = USIOJConvert::ToJsonString(Response);
			}

			if (SafeCallback)
			{
				SafeCallback(Result);
			}
		});
}

void UMachineLearningRemoteComponent::SendRawInput(const TArray<float>& InputData, const FString& FunctionName /*= TEXT("onFloatArrayInput")*/)
{
	const FString SafeFunctionName = FunctionName;
	SendRawInput(InputData, [this, SafeFunctionName](TArray<float>& ResultData)
	{
		OnRawInputResult.Broadcast(ResultData, SafeFunctionName);
	}, FunctionName);
}

void UMachineLearningRemoteComponent::SendRawInput(const TArray<float>& InputData, TFunction<void(TArray<float>& ResultData)> ResultCallback, const FString& FunctionName /*= TEXT("onFloatArrayInput")*/)
{
	//Embed data in a ustruct, this will get auto-serialized into a python/json object on other side
	FMLSendRawObject SendObject;
	SendObject.InputData = InputData;
	SendObject.TargetFunction = FunctionName;

	const auto SafeCallback = ResultCallback;

	Socket->Emit(SendInputEventName, FMLSendRawObject::StaticStruct(), &SendObject, [this, FunctionName, SafeCallback](auto ResponseArray)
		{
			//UE_LOG(MLBaseLog, Log, TEXT("Got callback response: %s"), *USIOJConvert::ToJsonString(ResponseArray));
			if (ResponseArray.Num() == 0)
			{
				return;
			}

			//We only handle the one response for now
			TSharedPtr<FJsonValue> Response = ResponseArray[0];

			if (Response->Type != EJson::Object)
			{
				UE_LOG(MLBaseLog, Warning, TEXT("SendRawInput: Expected float array wrapped object, got %s"), *USIOJConvert::ToJsonString(ResponseArray));
				return;
			}

			FMLSendRawObject ReceiveObject;
			USIOJConvert::JsonObjectToUStruct(Response->AsObject(), FMLSendRawObject::StaticStruct(), &ReceiveObject);

			if (SafeCallback != nullptr)
			{
				SafeCallback(ReceiveObject.InputData);
			}
		});
}

void UMachineLearningRemoteComponent::SendStringInputGraphCallback(const FString& InputData, FString& ResultData, struct FLatentActionInfo LatentInfo, const FString& FunctionName /*= TEXT("onJsonInput")*/)
{
	FCULatentAction* LatentAction = FCULatentAction::CreateLatentAction(LatentInfo, this);

	//Embed data in a ustruct, this will get auto-serialized into a python/json object on other side
	FMLSendStringObject SendObject;
	SendObject.InputData = InputData;
	SendObject.TargetFunction = FunctionName;

	Socket->Emit(SendInputEventName, FMLSendStringObject::StaticStruct(), &SendObject, [this, FunctionName, LatentAction, &ResultData](auto ResponseArray)
	{
		//UE_LOG(MLBaseLog, Log, TEXT("Got callback response: %s"), *USIOJConvert::ToJsonString(ResponseArray));
		if (ResponseArray.Num() == 0)
		{
			return;
		}

		//We only handle the one response for now
		TSharedPtr<FJsonValue> Response = ResponseArray[0];

		//Grab the value as a string
		//Todo: support non-string encoding?
		if (Response->Type == EJson::String)
		{
			ResultData = Response->AsString();
		}
		else
		{
			ResultData = USIOJConvert::ToJsonString(Response);
		}

		LatentAction->Call();	//resume the latent action
	});
}

void UMachineLearningRemoteComponent::SendRawInputGraphCallback(const TArray<float>& InputData, TArray<float>& ResultData, struct FLatentActionInfo LatentInfo, const FString& FunctionName /*= TEXT("onJsonInput")*/)
{
	FCULatentAction* LatentAction = FCULatentAction::CreateLatentAction(LatentInfo, this);

	//Embed data in a ustruct, this will get auto-serialized into a python/json object on other side
	FMLSendRawObject SendObject;
	SendObject.InputData = InputData;
	SendObject.TargetFunction = FunctionName;

	Socket->Emit(SendInputEventName, FMLSendRawObject::StaticStruct(), &SendObject, [this, FunctionName, LatentAction, &ResultData](auto ResponseArray)
	{
		//UE_LOG(MLBaseLog, Log, TEXT("Got callback response: %s"), *USIOJConvert::ToJsonString(ResponseArray));
		if (ResponseArray.Num() == 0)
		{
			return;
		}

		//We only handle the one response for now
		TSharedPtr<FJsonValue> Response = ResponseArray[0];

		if (Response->Type != EJson::Object)
		{
			UE_LOG(MLBaseLog, Warning, TEXT("SendRawInputGraphCallback: Expected float array wrapped object, got %s"), *USIOJConvert::ToJsonString(ResponseArray));
			return;
		}

		FMLSendRawObject ReceiveObject;
		USIOJConvert::JsonObjectToUStruct(Response->AsObject(), FMLSendRawObject::StaticStruct(), &ReceiveObject);

		ResultData = ReceiveObject.InputData;

		LatentAction->Call();	//resume the latent action
	});
}


void UMachineLearningRemoteComponent::SetObservationSpace( int32 Value )
{
	ObservationSpace = Value;
}


void UMachineLearningRemoteComponent::SetRLAgentType( FString Type )
{
	RLAgentType = Type;
}

void UMachineLearningRemoteComponent::TargetServerConnect()
{
	//Setup callbacks
	Socket = ISocketIOClientModule::Get().NewValidNativePointer();
	Socket->OnConnectedCallback = [this](const FString& InSessionId)
	{
		if (Socket.IsValid())
		{
			bIsConnectedToBackend = true;
			OnConnectedToBackend.Broadcast(InSessionId);

			if (bStartScriptOnConnection)
			{
				SendObservationSpace();
				SendAgentType();
				StartScript(DefaultScript);
			}
		}
	};

	Socket->OnDisconnectedCallback = [this](const ESIOConnectionCloseReason Reason)
	{
		if (Socket.IsValid())
		{
			bIsConnectedToBackend = false;
			OnDisconnectedFromBackend.Broadcast(Socket->LastSessionId);
		}
	};

	Socket->OnNamespaceConnectedCallback = [this](const FString& Namespace)
	{
		if (Socket.IsValid())
		{
		}
	};

	Socket->OnNamespaceDisconnectedCallback = [this](const FString& Namespace)
	{
		if (Socket.IsValid())
		{

		}
	};
	Socket->OnReconnectionCallback = [this](const uint32 AttemptCount, const uint32 DelayInMs)
	{
		if (Socket.IsValid())
		{
		}
	};

	Socket->OnFailCallback = [this]()
	{
		if (Socket.IsValid())
		{

		};
	};
	Socket->OnEvent(ScriptStartedEventName, [this](const FString& EventName, const TSharedPtr<FJsonValue>& Params)
		{
			bScriptRunning = true;
			OnScriptStarted.Broadcast(Params->AsString());
		});

	Socket->OnEvent(LogEventName, [this](const FString& EventName, const TSharedPtr<FJsonValue>& Params)
		{
			OnLog.Broadcast(Params->AsString());
		});

	if (bConnectOnBeginPlay)
	{
		Socket->Connect(ServerAddressAndPort);
	}
}