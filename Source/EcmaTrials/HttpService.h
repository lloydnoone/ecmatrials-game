// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HttpService.generated.h"

// structs, both used to pass data back and forth between UE4 and your Back-End Server
//FRequest_Login holds the email and password that we are using to log into our account.
// JSON EXAMPLE: { "email":"some@email.com", "password":"strongpassword" }
//USTRUCT()
//struct FRequest_Login {
//	GENERATED_BODY()
//	UPROPERTY() FString email;
//	UPROPERTY() FString password;
//
//	FRequest_Login() {}
//};

// FResponse_Login holds the returned response from the login request when it comes in.
// JSON EXAMPLE: { "id":1, "name":"Batman", "hash":"asdf-qwer-dfgh-erty" }
//USTRUCT()
//struct FResponse_Login {
//	GENERATED_BODY()
//	UPROPERTY() int id;
//	UPROPERTY() FString name;
//	UPROPERTY() FString hash; // basically  cookie
//
//	FResponse_Login() {}
//};

USTRUCT()
struct FError_PostCode {
	GENERATED_BODY()
	UPROPERTY() FString message;
	UPROPERTY() FString name;
	UPROPERTY() FString stack;
	UPROPERTY() FString LineNumber;

	FError_PostCode() {}
};

USTRUCT()
struct FRequest_PostCode {
	GENERATED_BODY()
	UPROPERTY() FString snippet;

	FRequest_PostCode() {}
};

USTRUCT()
struct FResponse_PostCode {
	GENERATED_BODY()
	UPROPERTY() FString message;
	UPROPERTY() FError_PostCode error;

	FResponse_PostCode() {}
};

class UCodeEditor;


UCLASS(Blueprintable)
class ECMATRIALS_API AHttpService : public AActor
{
	GENERATED_BODY()
	
private:
	UCodeEditor* CodeEditor; // a reference to the editor that made a request
	FHttpModule* Http; // Holds a reference to UE4's Http implementation. It's used to create request objects.
	FString ApiBaseUrl = "https://www.ecmatrials.com/api/";

	FString AuthorizationHeader = TEXT("Authorization"); //  This is the key for the Authentication header. Your back-end might expect a different form of this such as X-Auth-Token, X-Requested-With or something similar.
	FString AuthorizationHash = TEXT("asdfasdf");
	void SetAuthorizationHash(FString Hash);

	// Both RequestWithRoute and SetRequestHeaders are used to initialize Http Request Objects. They shouldn't be called directly, only through the methods below.
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> RequestWithRoute(FString Subroute);
	void SetRequestHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request);

	// these are the methods that should be called to use this class
	// better to have different methods for each request type as passing it as a string is sloppy and error prone.
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetRequest(FString Subroute);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> PostRequest(FString Subroute, FString ContentJsonString);
	void Send(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request);

	bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

	template <typename StructType>
	void GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput);
	template <typename StructType>
	void GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput);
public:
	AHttpService();
	virtual void BeginPlay() override;

	/*void Login(FRequest_Login LoginCredentials);
	void LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);*/

	void PostCode(FRequest_PostCode Code, UCodeEditor* CurrentEditor);
	void PostCodeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
