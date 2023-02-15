// Copyright MacroHard Systems.


#include "TutorialCard.h"
#include "Components/WidgetComponent.h"
#include "TutorialText.h"
#include "Internationalization/StringTable.h"
#include "Components/BoxComponent.h"
#include "EcmaCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATutorialCard::ATutorialCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(Root);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CollisionBox->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ATutorialCard::BeginPlay()
{
	Super::BeginPlay();
	
	TutorialTextWidget = Cast<UTutorialText>(Widget->GetWidget());
	if (!TutorialTextWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("couldnt cast to UTutorialWidget"));
	}
	else if (Text.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Text in TutorialCard is empty"));
	}
	else
	{
		TutorialTextWidget->SetText(Text);
	}

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATutorialCard::BeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ATutorialCard::EndOverlap);
}

// Called every frame
void ATutorialCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATutorialCard::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);

	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Begin Overlap Player reference is null"));
		return;
	}

	// dont do anything if it wasnt the player that overlapped
	if (!(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == Player)) return;

	// play fade in and become visible
	TutorialTextWidget->AnimatedVisible(true);
}

void ATutorialCard::EndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);

	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap Player reference is null in tutorial card"));
		return;
	}
	if (!this)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap this reference is null in tutorial card"));
		return;
	}

	// dont do anything if it wasnt the player that ended overlap
	if (!(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == Player)) return;

	// fade out and become invisible
	TutorialTextWidget->AnimatedVisible(false);
}

void ATutorialCard::AnimatedVisible(bool bVisible)
{
	if (!TutorialTextWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock is null in TutorialText Widget, this maybe because it needs to be added to the viewport to init"));
		return;
	}

	TutorialTextWidget->AnimatedVisible(true);
}

