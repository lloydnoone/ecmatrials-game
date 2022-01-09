// Copyright Prestige Games World Wide.

//For the sake of clean code you should not be doing any non - http logic here.Pass your response data somewhere elseand handle it there.
//APIs tend to be quite large andif you put all of your logic inside of your HttpService it will be too large to handle in the future.

#include "HttpService.h"
#include "CodeEditor.h"

AHttpService::AHttpService() { PrimaryActorTick.bCanEverTick = false; }

void AHttpService::BeginPlay() {
	Super::BeginPlay();

	// We don't want clients to be able to run HTTP requests. Only servers.
	if (!HasAuthority()) return;
	Http = &FHttpModule::Get();

	/*FRequest_Login LoginCredentials;
	LoginCredentials.email = TEXT("asdf@asdf.com");
	LoginCredentials.password = TEXT("asdfasdf");
	Login(LoginCredentials);*/
}




/**********************************************************************************************************************************************/




TSharedRef<IHttpRequest, ESPMode::ThreadSafe> AHttpService::RequestWithRoute(FString Subroute) {
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(ApiBaseUrl + Subroute);
	SetRequestHeaders(Request);
	return Request;
}

void AHttpService::SetRequestHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request) {
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	Request->SetHeader(AuthorizationHeader, AuthorizationHash); // defaults set in header
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> AHttpService::GetRequest(FString Subroute) {
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = RequestWithRoute(Subroute);
	Request->SetVerb("GET");
	return Request;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> AHttpService::PostRequest(FString Subroute, FString ContentJsonString) {
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = RequestWithRoute(Subroute);
	Request->SetVerb("POST");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

void AHttpService::Send(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request) {
	Request->ProcessRequest();
}

// ResponseIsValid is used to deeply check if a response is valid.
bool AHttpService::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	else {
		UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
		return false;
	}
}

void AHttpService::SetAuthorizationHash(FString Hash/*, TSharedRef<IHttpRequest>& Request*/) {
	AuthorizationHash = Hash;
}



/**********************************************************************************************************************************************/

// these 2 convert json strings to and from the closest thing unreal has to json objects, which is the structs defined in the .h
// uses out params

template <typename StructType>
void AHttpService::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

template <typename StructType>
void AHttpService::GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput) {
	StructType StructData;
	FString JsonString = Response->GetContentAsString();
	FJsonObjectConverter::JsonObjectStringToUStruct<StructType>(JsonString, &StructOutput, 0, 0);
}



/**********************************************************************************************************************************************/



//void AHttpService::Login(FRequest_Login LoginCredentials) {
//	// get the json string form struct si its ready to be used in the request
//	FString ContentJsonString;
//	GetJsonStringFromStruct<FRequest_Login>(LoginCredentials, ContentJsonString);
//
//	// create request
//	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = PostRequest("user/login", ContentJsonString);
//	Request->OnProcessRequestComplete().BindUObject(this, &AHttpService::LoginResponse);
//
//	// and then send
//	Send(Request);
//}
//
////  all the parans for this are passed in automatically by unreal to this bound function
//void AHttpService::LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
//	// stop running if response was bad.
//	if (!ResponseIsValid(Response, bWasSuccessful))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Response was not valid"));
//		return;
//	}
//
//	// get data string from response and put it in a useful, safe struct
//	FResponse_Login LoginResponse;
//	GetStructFromJsonString<FResponse_Login>(Response, LoginResponse);
//
//	// We can set the hash for further requests here. Really though, we should be passing it back to the specific Player and bind it on each request
//	// otherwise every player will be the same user.
//	SetAuthorizationHash(LoginResponse.hash);
//
//	UE_LOG(LogTemp, Warning, TEXT("Id is: %d"), LoginResponse.id);
//	UE_LOG(LogTemp, Warning, TEXT("Name is: %s"), *LoginResponse.name);
//}

void AHttpService::PostCode(FRequest_PostCode Code, UCodeEditor* CurrentEditor, FString RequestUrl) {

	if (RequestUrl == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("no sub route defined in code editors request url for this actor"));
	}
	
	//set the editor that made the request
	CodeEditor = CurrentEditor;

	// get the json string form struct si its ready to be used in the request
	FString ContentJsonString;
	GetJsonStringFromStruct<FRequest_PostCode>(Code, ContentJsonString);

	// create request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = PostRequest(RequestUrl, ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpService::PostCodeResponse);

	// and then send
	Send(Request);
}

// these params are passed in b onProcessRequestComplete
void AHttpService::PostCodeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	// stop running if response was bad.
	if (!ResponseIsValid(Response, bWasSuccessful))
	{
		UE_LOG(LogTemp, Warning, TEXT("Response was not valid"));
		return;
	}

	// get data string from response and put it in a useful, safe struct
	FResponse_PostCode PostCodeResponse;
	GetStructFromJsonString<FResponse_PostCode>(Response, PostCodeResponse);

	CodeEditor->ReceiveResponse(PostCodeResponse);
}

