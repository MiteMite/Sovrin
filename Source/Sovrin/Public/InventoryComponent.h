// Penis 8===D ~~~~~

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentInterface.h"
#include "Pickupable.h"
#include "InventoryComponent.generated.h"

USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TScriptInterface<IEquipmentInterface> Equipment;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bIsEquipped = false;

	FInventorySlot()
	{
		Equipment = nullptr;
		Quantity = 1;
		bIsEquipped = false;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOVRIN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxInventorySize = 50;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TMap<EEquipmentSlot, TScriptInterface<IEquipmentInterface>> EquippedItems;

public:
	UFUNCTION(Category = "Inventory")
	bool AddItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity = 1);

	UFUNCTION(Category = "Inventory")
	bool RemoveItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity = 1);

	UFUNCTION(Category = "Inventory")
	bool HasItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity = 1) const;

	UFUNCTION(Category = "Inventory")
	TArray<FInventorySlot> GetInventoryItems() const {return InventorySlots;};

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetAvailableSlots() const;

	//Equipment Management
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool EquipItem(TScriptInterface<IEquipmentInterface> Equipment);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UnequipItem(EEquipmentSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TScriptInterface<IEquipmentInterface> GetEquippedItem(EEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<TScriptInterface<IEquipmentInterface>> GetAllEquippedItems() const;

	//filtering and searching
	UFUNCTION(Category = "Inventory")
	TArray<FInventorySlot> GetItemsByType(EEquipmentType Type) const;

	UFUNCTION(Category = "Inventory")
	TArray<FInventorySlot> GetEquippableItems(EEquipmentSlot Slot) const;

protected:
	virtual void BeginPlay() override;

private:
	int32 FindItemSlot(TScriptInterface<IEquipmentInterface> Equipment) const;
	bool CanAddItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity) const;
};
