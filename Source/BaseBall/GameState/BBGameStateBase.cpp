//BBGameStateBase.cpp

#include "GameState/BBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/BBPlayerController.h"

void ABBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABBPlayerController* BBPC = Cast<ABBPlayerController>(PC);
			if (IsValid(BBPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BBPC->PrintChatMessage(NotificationString);
			}
		}
	}
}
