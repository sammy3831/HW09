// BBGameModeBase.cpp

#include "GameMode/BBGameModeBase.h"
#include "EngineUtils.h"
#include "GameState/BBGameStateBase.h"
#include "PlayerController/BBPlayerController.h"
#include "PlayerState/BBPlayerState.h"

void ABBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABBPlayerController* BBPlayerController = Cast<ABBPlayerController>(NewPlayer);
	if (IsValid(BBPlayerController) == true)
	{
		BBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BBPlayerController);

		ABBPlayerState* BBPlayerState = BBPlayerController->GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPlayerState) == true)
		{
			BBPlayerState->PlayerName = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABBGameStateBase* BBGameStateBase = GetGameState<ABBGameStateBase>();
		if (IsValid(BBGameStateBase) == true)
		{
			BBGameStateBase->MulticastRPCBroadcastLoginMessage(BBPlayerState->PlayerName);
		}
	}
}

FString ABBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; i++)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do
	{
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;
	}
	while (false);

	return bCanPlay;
}

FString ABBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; i++)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("SecretNumberString: %s"), *SecretNumberString);
}

void ABBGameModeBase::PrintChatMessage(ABBPlayerController* InChattingPlayerController, const FString& InChatMessage)
{
	FString ChatMessage = InChatMessage;
	int Index = InChatMessage.Len() - 3;
	FString GuessNumberString = InChatMessage.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)
		{
			ABBPlayerController* BBPlayerController = *It;
			if (IsValid(BBPlayerController) == true)
			{
				FString CombinedMessage = InChatMessage + TEXT(" -> ") + JudgeResultString;
				BBPlayerController->ClientRPCPrintChatMessage(CombinedMessage);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)
		{
			ABBPlayerController* BBPlayerController = *It;
			if (IsValid(BBPlayerController) == true)
			{
				BBPlayerController->ClientRPCPrintChatMessage(InChatMessage);
			}
		}
	}
}

void ABBGameModeBase::IncreaseGuessCount(ABBPlayerController* InChattingPlayerController)
{
	ABBPlayerState* BBPlayerState = InChattingPlayerController->GetPlayerState<ABBPlayerState>();
	if (IsValid(BBPlayerState) == true)
	{
		BBPlayerState->CurrentGuessCount++;
	}
}

void ABBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& BBPlayerController : AllPlayerControllers)
	{
		ABBPlayerState* BBPlayerState = BBPlayerController->GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPlayerState) == true)
		{
			BBPlayerState->CurrentGuessCount = 0;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("SecretNumberString: %s"), *SecretNumberString);
}

void ABBGameModeBase::JudgeGame(ABBPlayerController* InChattingPlayerController, int32 InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABBPlayerState* BBPlayerState = InChattingPlayerController->GetPlayerState<ABBPlayerState>();
		for (const auto& BBPlayerController : AllPlayerControllers)
		{
			if (IsValid(BBPlayerState) == true)
			{
				FString CombinedMessage = BBPlayerState->PlayerName + TEXT(" has won the game.");
				BBPlayerController->NotificationText = FText::FromString(CombinedMessage);
			}
		}
		ResetGame();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BBPlayerController : AllPlayerControllers)
		{
			ABBPlayerState* BBPlayerState = BBPlayerController->GetPlayerState<ABBPlayerState>();
			if (IsValid(BBPlayerState) == true)
			{
				if (BBPlayerState->CurrentGuessCount < BBPlayerState->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& BBPlayerController : AllPlayerControllers)
			{
				BBPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
			}
			ResetGame();
		}
	}
}
