// 


#include "Sovrin/Public/SovrinHUD.h"

#include "SovrinMenuWidget.h"
#include "ToolBuilderUtil.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Sovrin/Public/MainMenuWidget.h"
#include "Sovrin/Public/PauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"

ASovrinHUD::ASovrinHUD()
{
	MainHUDWidgetClass = UMainMenuWidget::StaticClass();
	PauseMenuWidgetClass = USovrinMenuWidget::StaticClass();
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
	// Define button positions and dimensions
	const FVector2D ResumeButtonPosition(400.0f, 300.0f);
	const FVector2D OptionsButtonPosition(400.0f, 360.0f);
	const FVector2D MainMenuButtonPosition(400.0f, 420.0f);
	const FVector2D QuitButtonPosition(400.0f, 480.0f);

	const FVector2D ButtonSize(200.0f, 40.0f);
	
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

	if (bIsGamePaused)
	{
		// Draw buttons (rectangles for now)
		DrawButton(ResumeButtonPosition, ButtonSize, FLinearColor::Green, TEXT("Resume"));
		DrawButton(OptionsButtonPosition, ButtonSize, FLinearColor::Blue, TEXT("Options"));
		DrawButton(MainMenuButtonPosition, ButtonSize, FLinearColor::Yellow, TEXT("Main Menu"));
		DrawButton(QuitButtonPosition, ButtonSize, FLinearColor::Red, TEXT("Quit"));

		// Add interactions (mouse click detection)
		HandleButtonClick(ResumeButtonPosition, ButtonSize, TEXT("Resume"));
		HandleButtonClick(OptionsButtonPosition, ButtonSize, TEXT("Options"));
		HandleButtonClick(MainMenuButtonPosition, ButtonSize, TEXT("Main Menu"));
		HandleButtonClick(QuitButtonPosition, ButtonSize, TEXT("Quit"));
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Main HUD widget class not set"));
	}

	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pause menu widget class not set"));
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
		// Unpause the game
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		
		// Hide mouse cursor
		if (APlayerController* PC = GetOwningPlayerController())
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
		PauseMenuWidget->RemoveFromParent();
		bIsGamePaused = false;
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
	// Simple minimap placeholder
	constexpr float MapSize = 150.0f;
	const FVector2D MapPosition(Canvas->ClipX - MapSize - 20, 20);
	
	// Minimap background
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f), MapPosition.X, MapPosition.Y, MapSize, MapSize);
	
	// Minimap border
	DrawLine(MapPosition.X, MapPosition.Y, MapPosition.X + MapSize, MapPosition.Y, FLinearColor::White, 2.0f);
	DrawLine(MapPosition.X + MapSize, MapPosition.Y, MapPosition.X + MapSize, MapPosition.Y + MapSize, FLinearColor::White, 2.0f);
	DrawLine(MapPosition.X + MapSize, MapPosition.Y + MapSize, MapPosition.X, MapPosition.Y + MapSize, FLinearColor::White, 2.0f);
	DrawLine(MapPosition.X, MapPosition.Y + MapSize, MapPosition.X, MapPosition.Y, FLinearColor::White, 2.0f);

	// Get player location
	FVector PlayerLocation = FVector::ZeroVector;
	if (APawn* PlayerPawn = GetOwningPawn())
	{
		PlayerLocation = PlayerPawn->GetActorLocation();
	}

	// NME AI dot
	DrawEnemiesOnMinimap(MapPosition, MapSize, PlayerLocation);
	
	// Player dot in center
	const FVector2D PlayerPos(MapPosition.X + MapSize * 0.5f, MapPosition.Y + MapSize * 0.5f);
	DrawRect(FLinearColor::Blue, PlayerPos.X - 3, PlayerPos.Y - 3, 6, 6);
}

void ASovrinHUD::DrawEnemiesOnMinimap(const FVector2D& MapPosition, float MapSize, const FVector& PlayerLocation)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	// Find all actors with "Enemy" tag
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsWithTag(World, FName("Enemy"), EnemyActors);

	for (AActor* Enemy : EnemyActors)
	{
		if (!Enemy)
			continue;

		FVector EnemyLocation = Enemy->GetActorLocation();
		FVector RelativeLocation = EnemyLocation - PlayerLocation;

		// Check if enemy is within minimap range
		float Distance = RelativeLocation.Size();
		if (Distance > MinimapRange)
			continue;

		// Convert 3D world coordinates to 2D minimap coordinates
		float ScaledX = (-RelativeLocation.Y * MinimapScale);
		float ScaledY = (-RelativeLocation.X * MinimapScale);

		// Clamp to minimap boundaries
		float HalfMapSize = MapSize * 0.5f;
		ScaledX = FMath::Clamp(ScaledX, -HalfMapSize, HalfMapSize);
		ScaledY = FMath::Clamp(ScaledY, -HalfMapSize, HalfMapSize);

		// Calculate final screen position
		FVector2D EnemyScreenPos;
		EnemyScreenPos.X = MapPosition.X + HalfMapSize + ScaledX;
		EnemyScreenPos.Y = MapPosition.Y + HalfMapSize - ScaledY;
		
		// Draw enemy dot
		float HalfDotSize = EnemyDotSize * 0.5f;
		DrawRect(EnemyDotColor, 
			EnemyScreenPos.X - HalfDotSize, 
			EnemyScreenPos.Y - HalfDotSize, 
			EnemyDotSize, 
			EnemyDotSize);
	}
}

void ASovrinHUD::DrawButton(FVector2D Position, FVector2D Size, FLinearColor Color, const FString& ButtonText)
{
	// Draw rectangle as button background
	DrawRect(Color, Position.X, Position.Y, Size.X, Size.Y);

	// Draw button text
	const float TextScale = 1.5f;
	DrawText(ButtonText, FLinearColor::White, Position.X + 20.0f, Position.Y + 5.0f, nullptr, TextScale);
}

void ASovrinHUD::HandleButtonClick(FVector2D Position, FVector2D Size, const FString& ButtonIdentifier)
{
	if (!IsMouseClickedInRegion(Position, Size))
	{
		return;
	}

	if (ButtonIdentifier == "Resume")
	{
		UE_LOG(LogTemp, Display, TEXT("Resume Button Clicked"));
		HidePauseMenu();
	}
	else if (ButtonIdentifier == "Options")
	{
		UE_LOG(LogTemp, Display, TEXT("Options Button Clicked"));
		// Open options menu
	}
	else if (ButtonIdentifier == "Main Menu")
	{
		UE_LOG(LogTemp, Display, TEXT("Main Menu Button Clicked"));
		UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuLevel"));
	}
	else if (ButtonIdentifier == "Quit")
	{
		UE_LOG(LogTemp, Display, TEXT("Quit Button Clicked"));
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
	}
}

bool ASovrinHUD::IsMouseClickedInRegion(FVector2D Position, FVector2D Size)
{
	// Access player controller
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return false;
	}

	float MouseX, MouseY;
	if (!PC->GetMousePosition(MouseX, MouseY))
	{
		return false;
	}

	// Check if mouse position is in button region
	return (MouseX > Position.X && MouseX < Position.X + Size.X &&
			MouseY > Position.Y && MouseY < Position.Y + Size.Y &&
			PC->WasInputKeyJustPressed(EKeys::LeftMouseButton));
}
