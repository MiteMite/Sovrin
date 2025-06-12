// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SovrinHUD.generated.h"

/**
 * 
 */
UCLASS()
class SOVRIN_API ASovrinHUD : public AHUD
{
	GENERATED_BODY()
public:
	ASovrinHUD();
	
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	
	// HUD Widget References
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UUserWidget* MainHUDWidget;

	// Menu System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Menu")
	UUserWidget* PauseMenuWidget;

	// HUD Functions
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowMainHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideMainHUD();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DrawEnemiesOnMinimap(const FVector2D& Location, float MapSize, const FVector& PlayerPosition);
	// Game State
	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	bool bIsGamePaused = false;

protected:
	// HUD Drawing Functions
	void DrawCrosshair();
	void DrawHealthBar();
	void DrawMiniMap();
	void DrawButton(FVector2D Position, FVector2D Size, FLinearColor Color, const FString& ButtonText);
	void HandleButtonClick(FVector2D Position, FVector2D Size, const FString& ButtonIdentifier);
	bool IsMouseClickedInRegion(FVector2D Position, FVector2D Size);
	
	// HUD Properties
	UPROPERTY(EditAnywhere, Category = "HUD Display")
	bool bShowCrosshair = true;

	UPROPERTY(EditAnywhere, Category = "HUD Display")
	bool bShowHealthBar = true;

	UPROPERTY(EditAnywhere, Category = "HUD Display")
	bool bShowMiniMap = true;

	// Minimap properties
	UPROPERTY(EditAnywhere, Category = "Minimap")
	float MinimapScale = 0.1f; // How much world space fits in the minimap

	UPROPERTY(EditAnywhere, Category = "Minimap")
	float EnemyDotSize = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Minimap")
	FLinearColor EnemyDotColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = "Minimap")
	float MinimapRange = 2000.0f; // Maximum distance to show enemies

	// Crosshair properties
	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairSize = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	FLinearColor CrosshairColor = FLinearColor::White;

	// Health Bar properties
	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 100.0f;

private:
	void InitializeWidgets();

};
