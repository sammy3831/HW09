// ChattingUI.cpp

#include "UI/ChattingUI.h"

#include "Components/EditableTextBox.h"
#include "PlayerController/BBPlayerController.h"

void UChattingUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_Chatting->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_Chatting->OnTextCommitted.AddDynamic(this, &ThisClass::UChattingUI::OnChatInputTextCommitted);
	}
}

void UChattingUI::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_Chatting->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_Chatting->OnTextCommitted.RemoveDynamic(this, &ThisClass::UChattingUI::OnChatInputTextCommitted);
	}
}

void UChattingUI::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ABBPlayerController* OwningBBPlayerController = Cast<ABBPlayerController>(OwningPlayerController);
			if (IsValid(OwningBBPlayerController) == true)
			{
				OwningBBPlayerController->SetChatMessage(Text.ToString());
				EditableTextBox_Chatting->SetText(FText());
			}
		}
	}
}
