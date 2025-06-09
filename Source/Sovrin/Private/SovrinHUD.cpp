// 


#include "Sovrin/Public/SovrinHUD.h"

#include "ToolBuilderUtil.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ASovrinHUD::ASovrinHUD()
{
	InitializeWidgets();
}

void ASovrinHUD::BeginPlay()
{
	//do stuff here
	Super::BeginPlay();
	InitializeWidgets();
}

void ASovrinHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bShowCrosshair && !bIsGamePaused)
	{
		//DrawCrosshair();
	}

	if (bShowHealthBar && !bIsGamePaused)
	{
		DrawHealthBar();
	}

	if (bShowMiniMap && !bIsGamePaused)
	{
		DrawMiniMap();
	}
}

void ASovrinHUD::InitializeWidgets()
{
	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UUserWidget>(GetWorld(), MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}

	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
	}
}

void ASovrinHUD::ShowMainHUD()
{
	if (MainHUDWidget && !MainHUDWidget->IsInViewport())
	{
		MainHUDWidget->AddToViewport();
	}
}

void ASovrinHUD::HideMainHUD()
{
	if (MainHUDWidget && MainHUDWidget->IsInViewport())
	{
		MainHUDWidget->RemoveFromParent();
	}
}

void ASovrinHUD::ShowPauseMenu()
{
	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->AddToViewport(100); // High Z-order to appear on top
		bIsGamePaused = true;
		
		// Pause the game
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		
		// Show mouse cursor
		if (APlayerController* PC = GetOwningPlayerController())
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void ASovrinHUD::HidePauseMenu()
{
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromParent();
		bIsGamePaused = false;
		
		// Unpause the game
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		
		// Hide mouse cursor
		if (APlayerController* PC = GetOwningPlayerController())
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void ASovrinHUD::TogglePauseMenu()
{
	if (bIsGamePaused)
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void ASovrinHUD::DrawCrosshair()
{
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const float HalfSize = CrosshairSize * 0.5f;

	// Draw crosshair lines
	DrawLine(Center.X - HalfSize, Center.Y, Center.X + HalfSize, Center.Y, CrosshairColor, 2.0f);
	DrawLine(Center.X, Center.Y - HalfSize, Center.X, Center.Y + HalfSize, CrosshairColor, 2.0f);
}

void ASovrinHUD::DrawHealthBar()
{
	const float HealthPercentage = CurrentHealth / MaxHealth;
	const float BarWidth = 200.0f;
	const float BarHeight = 20.0f;
	const FVector2D BarPosition(50.0f, 50.0f);

	// Background
	DrawRect(FLinearColor::Black, BarPosition.X - 2, BarPosition.Y - 2, BarWidth + 4, BarHeight + 4);
	
	// Health bar background
	DrawRect(FLinearColor::Red, BarPosition.X, BarPosition.Y, BarWidth, BarHeight);
	
	// Current health
	DrawRect(FLinearColor::Green, BarPosition.X, BarPosition.Y, BarWidth * HealthPercentage, BarHeight);

	// Health text
	FString HealthText = FString::Printf(TEXT("Health: %.0f/%.0f"), CurrentHealth, MaxHealth);
	DrawText(HealthText, FLinearColor::White, BarPosition.X, BarPosition.Y + BarHeight + 5);
}

void ASovrinHUD::DrawMiniMap()
{
	// Simple minimap placeholder - you can expand this based on your needs
	constexpr float MapSize = 150.0f;
	const FVector2D MapPosition(Canvas->ClipX - MapSize - 20, 20);
	
	// Minimap background
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f), MapPosition.X, MapPosition.Y, MapSize, MapSize);
	
	// Minimap border
	DrawLine(MapPosition.X, MapPosition.Y, MapPosition.X + MapSize, MapPosition.Y, FLinearColor::White, 2.0f);
	DrawLine(MapPosition.X + MapSize, MapPosition.Y, MapPosition.X + MapSize, MapPosition.Y + MapSize, FLinearColor::White, 2.0f);
	DrawLine(MapPosition.X + MapSize, MapPosition.Y + MapSize, MapPosition.X, MapPosition.Y + MapSize, FLinearColor::White, 2.0f);
	DrawLine(MapPosition.X, MapPosition.Y + MapSize, MapPosition.X, MapPosition.Y, FLinearColor::White, 2.0f);

	
	// Player dot in center
	const FVector2D PlayerPos(MapPosition.X + MapSize * 0.5f, MapPosition.Y + MapSize * 0.5f);
	DrawRect(FLinearColor::Blue, PlayerPos.X - 3, PlayerPos.Y - 3, 6, 6);
}

