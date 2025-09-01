// BBPlayerController.cpp

#include "PlayerController/BBPlayerController.h"
#include "BaseBall.h"
#include "GameMode/BBGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/BBPlayerState.h"
#include "UI/ChattingUI.h"

ABBPlayerController::ABBPlayerController()
{
	bReplicates = true;
}

void ABBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChattingUIWidgetClass) == true)
	{
		ChattingUIWidgetInstance = CreateWidget<UChattingUI>(this, ChattingUIWidgetClass);
		if (IsValid(ChattingUIWidgetInstance) == true)
		{
			ChattingUIWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(ChattingUIWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ABBPlayerController::SetChatMessage(const FString& InChatMessage)
{
	ChatMessage = InChatMessage;

	if (IsLocalController() == true)
	{
		ABBPlayerState* BBPS = GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPS) == true)
		{
			FString CombinedMessage = BBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessage;

			ServerRPCPrintChatMessage(CombinedMessage);
		}
	}
}

void ABBPlayerController::PrintChatMessage(const FString& InChatMessage)
{
	BaseBallFunctionLibrary::MyPrintString(this, InChatMessage, 10.f);
}

void ABBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABBPlayerController::ClientRPCPrintChatMessage_Implementation(const FString& InChatMessage)
{
	PrintChatMessage(InChatMessage);
}

void ABBPlayerController::ServerRPCPrintChatMessage_Implementation(const FString& InChatMessage)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABBGameModeBase* BBGM = Cast<ABBGameModeBase>(GM);
		if (IsValid(BBGM) == true)
		{
			BBGM->PrintChatMessage(this, InChatMessage);
		}
	}
}
