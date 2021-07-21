// Copyright Prestige Games World Wide.


#include "HttpComponent.h"
#include "Containers/UnrealString.h"

// Sets default values
UHttpComponent::UHttpComponent()
{
	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
}

// Called when the game starts or when spawned
//void UHttpComponent::BeginPlay()
//{
//	MyHttpCall();
//	Super::BeginPlay();
//}

/*Http call*/
void UHttpComponent::PostCode(FString InputCode)
{
	if (!Http)
	{
		UE_LOG(LogTemp, Warning, TEXT("Http is nullptr"));
		return;
	}
	// create FString from inputcode
	const FString FormattedCode = FString::Printf(TEXT("{\"snippet\":\"%s\"}"), *InputCode);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UHttpComponent::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL("https://www.ecmatrials.com/api/tests/test");
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(*FormattedCode);
	Request->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("processing request"));
}

/*Assigned function on successfull http call*/
void UHttpComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("response recieved. "));
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		FString recievedString = JsonObject->GetStringField("message");
		UE_LOG(LogTemp, Warning, TEXT("from request %s"), *recievedString);

		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, recievedString);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("respone could not be deserialized."));
	}
}


