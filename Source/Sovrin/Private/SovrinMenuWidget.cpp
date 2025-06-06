// 

#include "Sovrin/Public/SovrinMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "SovrinHUD.h"
#include "Engine/World.h"

void USovrinMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindButtonEvents();
}

void USovrinMenuWidget::BindButtonEvents()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &USovrinMenuWidget::OnResumeClicked);
	}

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &USovrinMenuWidget::OnOptionsClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &USovrinMenuWidget::OnMainMenuClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &USovrinMenuWidget::OnQuitClicked);
	}
}

void USovrinMenuWidget::OnResumeClicked()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ASovrinHUD* SovrinHUD = Cast<ASovrinHUD>(PC->GetHUD()))
		{
			SovrinHUD->HidePauseMenu();
		}
	}
}

void USovrinMenuWidget::OnOptionsClicked()
{
	// Implement options menu functionality here
	// You could open another widget or show settings
}

void USovrinMenuWidget::OnMainMenuClicked()
{
	// Return to main menu level
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuLevel"));
}

void USovrinMenuWidget::OnQuitClicked()
{
	// Quit the application
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

