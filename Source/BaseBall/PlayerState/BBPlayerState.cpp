// BBPlayerState.cpp

#include "PlayerState/BBPlayerState.h"
#include "Net/UnrealNetwork.h"

ABBPlayerState::ABBPlayerState() :
	PlayerName(TEXT("None")),
	CurrentGuessCount(0),
	MaxGuessCount(3)
{
	bReplicates = true;
}

void ABBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerName);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
}

FString ABBPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerName + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
