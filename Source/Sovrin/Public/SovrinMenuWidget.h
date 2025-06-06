// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SovrinMenuWidget.generated.h"

UCLASS()
class SOVRIN_API USovrinMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// Menu Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// Menu Title
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MenuTitle;

	// Button Click Events
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnQuitClicked();

private:
	void BindButtonEvents();
};
